import random


class Request:
    """对应报告中的“乘客请求”数据结构"""

    def __init__(self, req_id, arrive_time, start_floor, target_floor):
        self.id = req_id
        self.arrive_time = arrive_time
        self.start_floor = start_floor
        self.target_floor = target_floor
        self.board_time = None  # 上车时间
        self.finish_time = None  # 到达目标楼层下车时间

    @property
    def wait_time(self):
        """等待时间 = 上车时间 - 请求到达时间"""
        if self.board_time is not None:
            return self.board_time - self.arrive_time
        return None


class ElevatorSimulator:
    def __init__(self, requests):
        # 1. 系统环境参数初始化
        self.requests = requests
        self.total_requests = len(requests)
        self.capacity = 8
        self.max_floor = 20

        # 2. 电梯状态初始化
        self.current_floor = 1
        self.current_time = 0
        self.direction = "STOP"  # 状态: "UP", "DOWN", "STOP"
        self.load = 0
        self.up_stops = set()  # 上行停靠集合
        self.down_stops = set()  # 下行停靠集合
        self.onboard = []  # 车内乘客列表

        # 3. 初始化等待队列 (按楼层分组并按到达时间排序)
        self.waiting = {i: [] for i in range(1, self.max_floor + 1)}
        for req in self.requests:
            self.waiting[req.start_floor].append(req)

        for floor in self.waiting:
            self.waiting[floor].sort(key=lambda r: r.arrive_time)

    def process_floor(self, floor):
        """处理到达事件：下客与上客"""
        alighted_count = 0
        boarded_count = 0

        # 1. 下车处理
        # 逆序遍历以便安全删除
        for req in reversed(self.onboard):
            if req.target_floor == floor:
                self.onboard.remove(req)
                self.load -= 1
                alighted_count += 1
                req.finish_time = self.current_time

        # 2. 上车处理
        waiting_list = self.waiting[floor]
        # 筛选符合条件且已到达的乘客
        candidates = [r for r in waiting_list if r.arrive_time <= self.current_time]

        for req in candidates:
            if self.load >= self.capacity:
                break  # 简单拒绝超载，留待下次

            # 判断乘客方向
            req_dir = "UP" if req.target_floor > req.start_floor else "DOWN"

            # 核心机制：如果电梯处于空闲停靠状态，首个上车乘客决定电梯后续方向
            if self.direction == "STOP":
                self.direction = req_dir

            # 判断方向是否一致（按需停靠机制）
            if self.direction == req_dir:
                # 接载乘客
                req.board_time = self.current_time
                self.load += 1
                boarded_count += 1
                self.onboard.append(req)
                waiting_list.remove(req)

                # 将目标楼层加入对应方向的停靠集合
                if req_dir == "UP":
                    self.up_stops.add(req.target_floor)
                else:
                    self.down_stops.add(req.target_floor)

        # 3. 更新当前时间（包含停靠耗时）
        # 报告要求：每次停靠总耗时 = 2秒 + 上下车人数×1秒
        if alighted_count > 0 or boarded_count > 0:
            stop_duration = 2 + (alighted_count * 1) + (boarded_count * 1)
            self.current_time += stop_duration

        # 4. 从停靠集合中移除当前楼层
        if floor in self.up_stops:
            self.up_stops.remove(floor)
        if floor in self.down_stops:
            self.down_stops.remove(floor)

    def run(self):
        """主模拟循环"""
        while True:
            # 终止条件：所有请求都已上车且车内无乘客
            if all(r.board_time is not None for r in self.requests) and not self.onboard:
                break

            # 状态检查：是否空闲
            if not self.up_stops and not self.down_stops:
                # 寻找尚未上车的、到达时间最早的请求
                unboarded = [r for r in self.requests if r.board_time is None]
                if not unboarded:
                    break
                next_req = min(unboarded, key=lambda r: r.arrive_time)

                # 如果请求还未发生，电梯原地等待
                if self.current_time < next_req.arrive_time:
                    self.current_time = next_req.arrive_time

                target_floor = next_req.start_floor

                # 决定前往接客的方向
                if target_floor > self.current_floor:
                    self.direction = "UP"
                    self.up_stops.add(target_floor)
                elif target_floor < self.current_floor:
                    self.direction = "DOWN"
                    self.down_stops.add(target_floor)
                else:
                    # 已经在请求发生的楼层
                    self.direction = "STOP"
                    self.process_floor(self.current_floor)
                continue

            # 状态检查：有任务，决定下一个停靠楼层
            next_floor = None
            if self.direction == "UP":
                # 找当前楼层上方的最近停靠点
                valid_stops = [f for f in self.up_stops if f > self.current_floor]
                if valid_stops:
                    next_floor = min(valid_stops)
                else:
                    # 前方无上行停靠点，准备掉头或停机
                    self.direction = "DOWN" if self.down_stops else "STOP"
                    continue
            elif self.direction == "DOWN":
                # 找当前楼层下方的最近停靠点
                valid_stops = [f for f in self.down_stops if f < self.current_floor]
                if valid_stops:
                    next_floor = max(valid_stops)
                else:
                    # 前方无下行停靠点，准备掉头或停机
                    self.direction = "UP" if self.up_stops else "STOP"
                    continue

            # 电梯移动
            travel_time = abs(next_floor - self.current_floor) * 1  # 速度 1层/秒
            self.current_time += travel_time
            self.current_floor = next_floor

            # 处理到达事件
            self.process_floor(self.current_floor)


# ---------------- 测试验证与数据生成 ----------------
def generate_requests(num_requests):
    requests = []
    for i in range(num_requests):
        arrive_time = random.randint(0, 100)  # 模拟100秒内随机产生的请求
        start_floor = random.randint(1, 20)
        target_floor = random.randint(1, 20)
        while target_floor == start_floor:
            target_floor = random.randint(1, 20)
        requests.append(Request(i + 1, arrive_time, start_floor, target_floor))
    return requests


if __name__ == "__main__":
    # 随机生成50个乘梯请求作为离线批处理数据
    test_requests = generate_requests(50)

    # infinite
    # test_requests = generate_requests(2e9)

    print(f"开始离线调度计算... (共 {len(test_requests)} 个请求)\n")

    # 初始化并运行仿真
    simulator = ElevatorSimulator(test_requests)
    simulator.run()

    # 统计结果
    total_wait_time = sum(req.wait_time for req in test_requests)
    avg_wait_time = total_wait_time / len(test_requests)

    # 输出部分日志验证
    for req in sorted(test_requests, key=lambda x: x.id)[:5]:  # 只打印前5个验证逻辑
        print(f"请求 {req.id:02d}: {req.start_floor:02d}层 -> {req.target_floor:02d}层 | "
              f"到达时间: {req.arrive_time:03d}秒 | 上车时间: {req.board_time:03d}秒 | "
              f"送达时间: {req.finish_time:03d}秒 | 等待时长: {req.wait_time:03d}秒")

    print(f"...\n")
    print("-" * 40)
    print(f"仿真结束总耗时: {simulator.current_time} 秒")
    print(f"优化目标: 平均等待时间为 {avg_wait_time:.2f} 秒")
    print("-" * 40)
