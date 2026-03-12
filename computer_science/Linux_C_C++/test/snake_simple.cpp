#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <coroutine>

using namespace std;
using namespace std::chrono;

// 游戏常量
const int WIDTH = 20;
const int HEIGHT = 20;
const int INITIAL_SPEED = 200000; // 微秒

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

// 简单的协程支持
struct SleepAwaiter {
    int microseconds;

    explicit SleepAwaiter(int usec) : microseconds(usec) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(coroutine_handle<> handle) const noexcept {
        thread([this, handle]() {
            this_thread::sleep_for(microseconds * 1us);
            handle.resume();
        }).detach();
    }

    void await_resume() const noexcept {}
};

struct SleepAwaitable {
    int microseconds;

    explicit SleepAwaitable(int usec) : microseconds(usec) {}

    SleepAwaiter operator co_await() const noexcept {
        return SleepAwaiter(microseconds);
    }
};

// 游戏协程的 Promise
struct GamePromise {
    GameState value;
    coroutine_handle<> previous = nullptr;

    auto initial_suspend() noexcept { return suspend_always{}; }
    auto final_suspend() noexcept { return suspend_never{}; }
    void return_value(GameState val) { value = val; }
    auto yield_value(GameState val) {
        value = val;
        return suspend_always{};
    }
    void unhandled_exception() { cout << "Game exception!" << endl; }
    auto get_return_object() { return coroutine_handle<GamePromise>::from_promise(*this); }
};

// GameTask
struct GameTask {
    using promise_type = GamePromise;
    coroutine_handle<promise_type> handle;

    GameTask(coroutine_handle<promise_type> h) : handle(h) {}
    ~GameTask() { if (handle) handle.destroy(); }

    bool await_ready() const noexcept { return false; }
    void await_suspend(coroutine_handle<> caller) const noexcept {
        handle.promise().previous = caller;
        handle.resume();
    }
    GameState await_resume() const noexcept { return handle.promise().value; }
};

// 全局变量
mutex game_mutex;
atomic<bool> running{true};
atomic<bool> game_updated{false};

// 生成食物
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
    if (pos.x < 1 || pos.x >= WIDTH - 1 || pos.y < 1 || pos.y >= HEIGHT - 1) {
        return true;
    }
    return find(snake.begin(), snake.end(), pos) != snake.end();
}

// 绘制游戏
void draw_game(const GameState& state) {
    lock_guard<mutex> lock(game_mutex);

    // 清屏
    cout << "\033[2J\033[H";

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

    // 显示信息
    cout << "Score: " << state.score << " | ";
    cout << "Speed: " << (1000000 / state.speed) << " fps" << endl;
    cout << "Controls: W=Up A=Left S=Down D=Right Q=Quit" << endl;

    game_updated = true;
}

// 游戏协程
GameTask game_coroutine() {
    GameState state;

    // 初始化蛇
    state.snake = {{WIDTH / 2, HEIGHT / 2}, {WIDTH / 2 - 1, HEIGHT / 2}, {WIDTH / 2 - 2, HEIGHT / 2}};
    state.food = generate_food(state.snake);
    state.direction = RIGHT;
    state.game_over = false;
    state.score = 0;
    state.speed = INITIAL_SPEED;

    while (!state.game_over && running) {
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
            break;
        }

        // 更新蛇身
        state.snake.insert(state.snake.begin(), new_head);

        // 检查是否吃到食物
        if (new_head == state.food) {
            state.score += 10;
            state.food = generate_food(state.snake);
            // 加快速度
            if (state.score % 30 == 0) {
                state.speed = max(state.speed - 20000, 80000);
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

// 输入处理函数
void input_handler(GameState& state) {
    char ch;
    while (running && cin.get(ch)) {
        if (ch == 'q' || ch == 'Q') {
            running = false;
            break;
        }

        Direction new_dir = state.direction;
        switch (ch) {
            case 'w': case 'W': new_dir = UP; break;
            case 's': case 'S': new_dir = DOWN; break;
            case 'a': case 'A': new_dir = LEFT; break;
            case 'd': case 'D': new_dir = RIGHT; break;
        }

        // 防止反向移动
        if ((new_dir == UP && state.direction != DOWN) ||
            (new_dir == DOWN && state.direction != UP) ||
            (new_dir == LEFT && state.direction != RIGHT) ||
            (new_dir == RIGHT && state.direction != LEFT)) {
            state.direction = new_dir;
        }

        // 清除输入缓冲区
        this_thread::sleep_for(10ms);
    }
}

int main() {
    cout << "Snake Game with Coroutines" << endl;
    cout << "Press Enter to start, Q to quit..." << endl;

    // 等待用户按下 Enter 开始
    cin.get();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // 启动游戏
    auto game = game_coroutine();

    // 启动输入处理线程
    GameState input_state;
    input_state.direction = RIGHT;
    thread input_thread(input_handler, std::ref(input_state));

    // 游戏主循环
    game.handle.resume();

    while (!game.handle.done() && running) {
        // 同步输入到游戏状态
        lock_guard<mutex> lock(game_mutex);
        game.handle.promise().value.direction = input_state.direction;

        game.handle.resume();
        this_thread::sleep_for(50ms);
    }

    // 等待线程结束
    running = false;
    input_thread.join();

    return 0;
}