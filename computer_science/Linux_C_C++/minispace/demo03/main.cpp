#include <iostream>
#include <deque>
#include <random>
#include <chrono>
#include <thread>
#include <queue>
#include <vector>
#include <optional>
#include <coroutine>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// -------------------- 终端原始模式设置 --------------------
class TerminalRawMode {
public:
    TerminalRawMode() {
        tcgetattr(STDIN_FILENO, &old_tio);
        struct termios new_tio = old_tio;
        new_tio.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

        // 设置非阻塞模式
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }
    ~TerminalRawMode() {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
        // 恢复阻塞模式（可选）
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
    }
private:
    struct termios old_tio;
};

// -------------------- 协程调度器 --------------------
class Scheduler {
public:
    using clock = std::chrono::steady_clock;

    static Scheduler& get() {
        static Scheduler instance;
        return instance;
    }

    void schedule(std::coroutine_handle<> h) {
        ready_queue.push(h);
    }

    void add_sleep(std::coroutine_handle<> h, clock::time_point until) {
        sleep_list.emplace_back(until, h);
    }

    void register_input_waiter(std::coroutine_handle<> h) {
        input_waiter = h;
    }

    void run() {
        running = true;
        while (running || !ready_queue.empty() || !sleep_list.empty() || input_waiter) {
            poll_input();
            process_sleep();

            if (!ready_queue.empty()) {
                auto h = ready_queue.front();
                ready_queue.pop();
                h.resume();
                if (h.done()) h.destroy();
            } else {
                // 没有就绪任务，短暂休眠以避免忙等
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

    void stop() { running = false; }

    std::queue<char> input_queue;   // 输入缓冲区
private:
    Scheduler() = default;

    void poll_input() {
        char ch;
        while (read(STDIN_FILENO, &ch, 1) == 1) {
            input_queue.push(ch);
        }
        if (input_waiter && !input_queue.empty()) {
            ready_queue.push(*input_waiter);
            input_waiter.reset();
        }
    }

    void process_sleep() {
        auto now = clock::now();
        auto it = sleep_list.begin();
        while (it != sleep_list.end()) {
            if (it->first <= now) {
                ready_queue.push(it->second);
                it = sleep_list.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::queue<std::coroutine_handle<>> ready_queue;
    std::vector<std::pair<clock::time_point, std::coroutine_handle<>>> sleep_list;
    std::optional<std::coroutine_handle<>> input_waiter;
    bool running = false;
};

// -------------------- 任务 (task) --------------------
template<typename T = void>
struct task;

template<typename T>
struct task_promise {
    std::optional<T> result;
    std::coroutine_handle<> waiter;

    task<T> get_return_object() {
        return task<T>{std::coroutine_handle<task_promise>::from_promise(*this)};
    }

    std::suspend_always initial_suspend() noexcept { return {}; }

    struct final_awaiter {
        bool await_ready() noexcept { return false; }
        void await_suspend(std::coroutine_handle<task_promise> h) noexcept {
            if (h.promise().waiter)
                h.promise().waiter.resume();
        }
        void await_resume() noexcept {}
    };
    final_awaiter final_suspend() noexcept { return {}; }

    void unhandled_exception() { std::terminate(); }

    void return_value(T val) { result = std::move(val); }
};

template<typename T>
struct task {
    using promise_type = task_promise<T>;
    using handle_type = std::coroutine_handle<promise_type>;

    task(handle_type h) : coro(h) {}
    ~task() { if (coro && coro.done()) coro.destroy(); }

    task(const task&) = delete;
    task& operator=(const task&) = delete;
    task(task&& other) noexcept : coro(other.coro) { other.coro = nullptr; }
    task& operator=(task&& other) noexcept {
        if (this != &other) {
            if (coro && coro.done()) coro.destroy();
            coro = other.coro;
            other.coro = nullptr;
        }
        return *this;
    }

    bool await_ready() const noexcept { return coro && coro.done(); }

    void await_suspend(std::coroutine_handle<> waiter) noexcept {
        coro.promise().waiter = waiter;
    }

    void await_resume() {
        if constexpr (!std::is_void_v<T>) {
            if (coro.promise().result) return std::move(*coro.promise().result);
            else throw std::runtime_error("task result missing");
        }
    }

    void resume() { if (coro && !coro.done()) coro.resume(); }

    handle_type get_handle() const { return coro; }

private:
    handle_type coro;
};

template<>
struct task_promise<void> {
    std::coroutine_handle<> waiter;

    task<void> get_return_object() {
        return task<void>{std::coroutine_handle<task_promise>::from_promise(*this)};
    }

    std::suspend_always initial_suspend() noexcept { return {}; }

    struct final_awaiter {
        bool await_ready() noexcept { return false; }
        void await_suspend(std::coroutine_handle<task_promise> h) noexcept {
            if (h.promise().waiter)
                h.promise().waiter.resume();
        }
        void await_resume() noexcept {}
    };
    final_awaiter final_suspend() noexcept { return {}; }

    void unhandled_exception() { std::terminate(); }
    void return_void() {}
};

// -------------------- 睡眠等待器 --------------------
struct sleep_for_awaitable {
    using clock = Scheduler::clock;
    clock::duration duration;
    clock::time_point start;

    bool await_ready() noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) {
        start = clock::now();
        Scheduler::get().add_sleep(h, start + duration);
    }
    void await_resume() noexcept {}
};

sleep_for_awaitable sleep_for(std::chrono::milliseconds ms) {
    return {ms};
}

// -------------------- 输入等待器 --------------------
struct wait_for_input_awaitable {
    bool await_ready() noexcept {
        return !Scheduler::get().input_queue.empty();
    }
    void await_suspend(std::coroutine_handle<> h) {
        Scheduler::get().register_input_waiter(h);
    }
    char await_resume() {
        char ch = Scheduler::get().input_queue.front();
        Scheduler::get().input_queue.pop();
        return ch;
    }
};

wait_for_input_awaitable wait_for_input() { return {}; }

// -------------------- 游戏常量与状态 --------------------
constexpr int WIDTH = 20;
constexpr int HEIGHT = 15;
constexpr char EMPTY = ' ';
constexpr char SNAKE_BODY = 'O';
constexpr char FOOD = '*';

struct Point {
    int x, y;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

enum class Direction { UP, DOWN, LEFT, RIGHT };

std::atomic<bool> game_over{false};

class Game {
public:
    Game() : snake({ {WIDTH/2, HEIGHT/2}, {WIDTH/2-1, HEIGHT/2}, {WIDTH/2-2, HEIGHT/2} }),
             dir(Direction::RIGHT) {
        generate_food();
    }

    void generate_food() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis_x(0, WIDTH-1);
        std::uniform_int_distribution<> dis_y(0, HEIGHT-1);

        do {
            food = {dis_x(gen), dis_y(gen)};
        } while (std::find(snake.begin(), snake.end(), food) != snake.end());
    }

    bool move_snake() {
        Point new_head = snake.front();
        switch (dir) {
            case Direction::UP:    new_head.y--; break;
            case Direction::DOWN:  new_head.y++; break;
            case Direction::LEFT:  new_head.x--; break;
            case Direction::RIGHT: new_head.x++; break;
        }

        // 撞墙检查
        if (new_head.x < 0 || new_head.x >= WIDTH || new_head.y < 0 || new_head.y >= HEIGHT) {
            return false; // 游戏结束
        }

        bool ate_food = (new_head == food);

        if (ate_food) {
            snake.push_front(new_head);
            generate_food();
        } else {
            snake.push_front(new_head);
            snake.pop_back();
        }

        // 自身碰撞检查（新头部不能与除尾部外的身体重叠，但在移动后，尾部已弹出）
        auto it = std::find(snake.begin() + 1, snake.end(), snake.front());
        if (it != snake.end()) {
            return false; // 撞到自己
        }

        return true;
    }

    void change_dir(Direction new_dir) {
        // 禁止直接反向
        if ((dir == Direction::UP && new_dir != Direction::DOWN) ||
            (dir == Direction::DOWN && new_dir != Direction::UP) ||
            (dir == Direction::LEFT && new_dir != Direction::RIGHT) ||
            (dir == Direction::RIGHT && new_dir != Direction::LEFT)) {
            dir = new_dir;
        }
    }

    void render() {
        // 清屏 (使用 ANSI 转义)
        std::cout << "\033[2J\033[1;1H";

        // 绘制上边框
        std::cout << "+";
        for (int i = 0; i < WIDTH; ++i) std::cout << "-";
        std::cout << "+\n";

        // 绘制每一行
        for (int y = 0; y < HEIGHT; ++y) {
            std::cout << "|";
            for (int x = 0; x < WIDTH; ++x) {
                Point p{x, y};
                if (p == food) {
                    std::cout << FOOD;
                } else if (std::find(snake.begin(), snake.end(), p) != snake.end()) {
                    std::cout << SNAKE_BODY;
                } else {
                    std::cout << EMPTY;
                }
            }
            std::cout << "|\n";
        }

        // 绘制下边框
        std::cout << "+";
        for (int i = 0; i < WIDTH; ++i) std::cout << "-";
        std::cout << "+\n";
        std::cout << "Score: " << snake.size() - 3 << "\n";
        std::cout << "Use WASD to move, Q to quit.\n";
    }

    const std::deque<Point>& get_snake() const { return snake; }

private:
    std::deque<Point> snake;
    Point food;
    Direction dir;
};

// -------------------- 协程任务 --------------------
task<void> snake_movement(Game& game) {
    while (!game_over) {
        co_await sleep_for(std::chrono::milliseconds(200)); // 移动间隔

        if (game_over) break;

        if (!game.move_snake()) {
            game_over = true;
            std::cout << "\nGame Over! You hit something.\n";
            break;
        }

        game.render();
    }
}

task<void> input_handler(Game& game) {
    while (!game_over) {
        char ch = co_await wait_for_input();

        if (game_over) break;

        switch (ch) {
            case 'w': case 'W': game.change_dir(Direction::UP); break;
            case 's': case 'S': game.change_dir(Direction::DOWN); break;
            case 'a': case 'A': game.change_dir(Direction::LEFT); break;
            case 'd': case 'D': game.change_dir(Direction::RIGHT); break;
            case 'q': case 'Q': game_over = true; break;
            default: break;
        }
    }
}

task<void> game_main() {
    Game game;
    game.render();

    auto snake_task = snake_movement(game);
    auto input_task = input_handler(game);

    Scheduler::get().schedule(snake_task.get_handle());
    Scheduler::get().schedule(input_task.get_handle());

    co_await snake_task;  // 等待蛇移动任务完成
    co_await input_task;  // 等待输入任务完成
}

// -------------------- 主函数 --------------------
int main() {
    TerminalRawMode raw;  // 设置终端原始模式，析构时恢复

    auto& sched = Scheduler::get();
    sched.schedule(game_main().get_handle());
    sched.run();

    std::cout << "Thanks for playing!\n";
    return 0;
}
