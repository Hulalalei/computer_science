#include <coroutine>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <debug.hpp>
#include <optional>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace std::chrono_literals;

// 游戏常量
const int WIDTH = 20;
const int HEIGHT = 20;
const int INITIAL_SPEED = 100000; // 微秒

// 方向枚举
enum Direction {
    UP, DOWN, LEFT, RIGHT
};

// 位置结构体
struct Position {
    int x, y;

    Position(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// 游戏状态结构体
struct GameState {
    vector<Position> snake;
    Position food;
    Direction direction;
    bool game_over;
    int score;
    int speed;
};

// 自定义的 Awaiter 用于异步等待
class SleepAwaiter {
    int microseconds;

public:
    explicit SleepAwaiter(int usec) : microseconds(usec) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(coroutine_handle<> handle) const noexcept {
        // 使用线程模拟异步等待
        thread([this, handle]() {
            this_thread::sleep_for(microseconds * 1us);
            handle.resume();
        }).detach();
    }

    void await_resume() const noexcept {}
};

//  SleepAwaitable 类型
struct SleepAwaitable {
    int microseconds;

    explicit SleepAwaitable(int usec) : microseconds(usec) {}

    SleepAwaiter operator co_await() const noexcept {
        return SleepAwaiter(microseconds);
    }
};

// 协程的 Promise 类型
template<typename T>
struct GamePromise {
    T value;
    coroutine_handle<> previous = nullptr;

    // 初始时挂起
    auto initial_suspend() noexcept {
        return suspend_always{};
    }

    // 最终时直接返回
    auto final_suspend() noexcept {
        return suspend_never{};
    }

    // 返回值
    void return_value(T val) {
        value = val;
    }

    // yield 值
    auto yield_value(T val) {
        value = val;
        return suspend_always{};
    }

    // 异常处理
    void unhandled_exception() {
        cout << "Game exception occurred!" << endl;
    }

    // 获取返回对象
    auto get_return_object() {
        return coroutine_handle<GamePromise>::from_promise(*this);
    }
};

// Task 类型封装
template<typename T>
struct Task {
    using promise_type = GamePromise<T>;
    coroutine_handle<promise_type> handle;

    Task(coroutine_handle<promise_type> h) : handle(h) {}

    ~Task() {
        if (handle) handle.destroy();
    }

    // 拷贝构造和移动构造
    Task(const Task&) = delete;
    Task(Task&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }

    // 避免拷贝赋值
    Task& operator=(const Task&) = delete;
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }

    // 异步等待
    bool await_ready() const noexcept { return false; }

    void await_suspend(coroutine_handle<> caller) const noexcept {
        handle.promise().previous = caller;
        handle.resume();
    }

    T await_resume() const noexcept {
        return handle.promise().value;
    }
};

// 游戏任务
struct GameTask {
    coroutine_handle<GamePromise<GameState>> handle;

    GameTask(coroutine_handle<GamePromise<GameState>> h) : handle(h) {}

    void resume() {
        if (handle && !handle.done()) {
            handle.resume();
        }
    }

    bool is_done() const {
        return handle && handle.done();
    }
};

// 生成随机食物位置
Position generate_food(const vector<Position>& snake) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis_x(1, WIDTH - 2);
    uniform_int_distribution<> dis_y(1, HEIGHT - 2);

    Position food;
    do {
        food = Position(dis_x(gen), dis_y(gen));
    } while (find(snake.begin(), snake.end(), food) != snake.end());

    return food;
}

// 检查碰撞
bool check_collision(const Position& pos, const vector<Position>& snake) {
    // 检查墙壁碰撞
    if (pos.x < 1 || pos.x >= WIDTH - 1 || pos.y < 1 || pos.y >= HEIGHT - 1) {
        return true;
    }

    // 检查自身碰撞
    return find(snake.begin(), snake.end(), pos) != snake.end();
}

// 绘制游戏界面到控制台
mutex game_mutex;
condition_variable game_cv;
bool game_updated = false;

void draw_game(const GameState& state) {
    lock_guard<mutex> lock(game_mutex);

    // 清屏
    cout << "\033[2J\033[H"; // 清屏并移动光标到左上角

    // 绘制游戏
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                cout << "#";
            } else {
                bool is_snake = false;
                bool is_head = false;
                for (size_t i = 0; i < state.snake.size(); i++) {
                    if (state.snake[i].x == x && state.snake[i].y == y) {
                        is_snake = true;
                        if (i == 0) is_head = true;
                        break;
                    }
                }
                if (is_head) {
                    cout << "@";
                } else if (is_snake) {
                    cout << "o";
                } else if (state.food.x == x && state.food.y == y) {
                    cout << "*";
                } else {
                    cout << " ";
                }
            }
        }
        cout << endl;
    }

    // 显示分数和控制说明
    cout << "Score: " << state.score << " | ";
    cout << "Speed: " << (1000000 / state.speed) << " fps" << endl;
    cout << "Controls: W=Up A=Left S=Down D=Right Q=Quit" << endl;

    game_updated = true;
    game_cv.notify_one();
}

// 获取用户输入（非阻塞方式）
bool get_input(optional<Direction>& out_dir) {
    // 检查是否有键盘输入
    if (cin.rdbuf()->in_avail() > 0) {
        char ch;
        cin.get(ch);
        cin.putback(ch); // 放回去以便下次读取

        switch (ch) {
            case 'w':
            case 'W':
                out_dir = UP;
                return true;
            case 's':
            case 'S':
                out_dir = DOWN;
                return true;
            case 'a':
            case 'A':
                out_dir = LEFT;
                return true;
            case 'd':
            case 'D':
                out_dir = RIGHT;
                return true;
            case 'q':
            case 'Q':
                return false; // 使用q退出
            default:
                break;
        }
    }
    return true; // 无有效输入，继续当前方向
}

// 游戏主协程
Task<GameState> game_coroutine(GameState initial_state) {
    GameState state = initial_state;

    // 初始化蛇
    state.snake = {{WIDTH / 2, HEIGHT / 2}, {WIDTH / 2 - 1, HEIGHT / 2}, {WIDTH / 2 - 2, HEIGHT / 2}};
    state.food = generate_food(state.snake);
    state.direction = RIGHT;
    state.game_over = false;
    state.score = 0;
    state.speed = INITIAL_SPEED;

    while (!state.game_over) {
        // 获取输入
        optional<Direction> input_dir;
        if (!get_input(input_dir)) {
            // Q pressed, game over
            state.game_over = true;
            co_yield state;
            break;
        }
        if (input_dir.has_value()) {
            // 防止反向移动
            if ((input_dir.value() == UP && state.direction != DOWN) ||
                (input_dir.value() == DOWN && state.direction != UP) ||
                (input_dir.value() == LEFT && state.direction != RIGHT) ||
                (input_dir.value() == RIGHT && state.direction != LEFT)) {
                state.direction = input_dir.value();
            }
        }

        // 移动蛇头
        Position new_head = state.snake[0];
        switch (state.direction) {
            case UP: new_head.y--; break;
            case DOWN: new_head.y++; break;
            case LEFT: new_head.x--; break;
            case RIGHT: new_head.x++; break;
        }

        // 检查碰撞
        if (check_collision(new_head, state.snake)) {
            state.game_over = true;
            co_yield state;
            break;
        }

        // 更新蛇身
        state.snake.insert(state.snake.begin(), new_head);

        // 检查是否吃到食物
        if (new_head == state.food) {
            state.score += 10;
            state.food = generate_food(state.snake);
            // 每得50分加快速度
            if (state.score % 50 == 0) {
                state.speed = max(state.speed - 10000, 50000);
            }
        } else {
            state.snake.pop_back();
        }

        // 绘制游戏
        draw_game(state);

        // 异步等待
        co_await SleepAwaitable(state.speed);
    }

    // 游戏结束
    draw_game(state);
    cout << "\nGAME OVER! Final Score: " << state.score << endl;

    co_yield state;
}

// 游戏任务管理器
GameTask start_game() {
    return GameTask(coroutine_handle<GamePromise<GameState>>::from_promise(
        *new GamePromise<GameState>{})
    );
}

// 更新游戏状态
void update_game_state(GameState& state) {
    // 从协程中获取最新的状态
    lock_guard<mutex> lock(game_mutex);
    if (game_updated) {
        // 这里可以添加更多的状态更新逻辑
        game_updated = false;
    }
}

// 输入处理线程
void input_thread(GameState& state, atomic<bool>& running) {
    while (running) {
        optional<Direction> input_dir;
        if (!get_input(input_dir)) {
            // Q pressed
            state.game_over = true;
            break;
        }
        if (input_dir.has_value()) {
            // 防止反向移动
            if ((input_dir.value() == UP && state.direction != DOWN) ||
                (input_dir.value() == DOWN && state.direction != UP) ||
                (input_dir.value() == LEFT && state.direction != RIGHT) ||
                (input_dir.value() == RIGHT && state.direction != LEFT)) {
                lock_guard<mutex> lock(game_mutex);
                state.direction = input_dir.value();
            }
        }
        this_thread::sleep_for(50ms);
    }
}

int main() {
    // 设置终端为非阻塞模式
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    // 初始化游戏状态
    GameState initial_state;
    initial_state.game_over = false;

    cout << "Snake Game with Coroutines" << endl;
    cout << "Press Enter to start, Q to quit..." << endl;

    // 等待用户按下 Enter 开始
    cin.get();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // 启动游戏协程
    GameTask game = start_game();
    game.handle.promise().value = initial_state;

    atomic<bool> running = true;
    thread input_thread_obj(input_thread, std::ref(initial_state), std::ref(running));

    // 游戏主循环
    while (!game.is_done() && !initial_state.game_over) {
        game.resume();
        if (game.handle.promise().value.game_over) {
            break;
        }
        this_thread::sleep_for(50ms);
    }

    // 停止输入线程
    running = false;
    input_thread_obj.join();

    // 显示最终结果
    cout << "\nGame Over! Final Score: " << game.handle.promise().value.score << endl;

    return 0;
}