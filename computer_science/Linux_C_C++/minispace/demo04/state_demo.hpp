#include <print>
#include <string>
#include <variant>


template <class... Ts> struct overload : Ts... {
  using Ts::operator()...;
};

struct Idle {
    auto operator<=>(const Idle&) const = default;
};

struct Running {
    int progress;
    auto operator<=>(const Running&) const = default;
};

struct Completed {
    std::string result;
    auto operator<=>(const Completed&) const = default;
};

struct Failed {
    std::string error;
    auto operator<=>(const Failed&) const = default;
};

using TaskState = std::variant<Idle, Running, Completed, Failed>;

void handle_state(const TaskState &state) {
    std::visit(overload{[](const Idle &) { std::println("waiting"); },
                  [](const Running &r) { std::println("progress: {}", r.progress); },
                  [](const Completed &c) { std::println("completed: {}", c.result); },
                  [](const Failed &f) { std::println("failed: {}", f.error); }},
        state);
}

int main() {
  handle_state(Idle{});
  handle_state(Running{42});
  handle_state(Completed{"success"});
  handle_state(Failed{"disk full"});

  return 0;
}
