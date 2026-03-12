package main

import (
    "fmt"
    "time"
)

func main() {
    // 启动一个协程
    go sayHello()

    // 主协程继续执行
    fmt.Println("主协程执行")

    // 等待足够时间让协程执行完成
    time.Sleep(time.Second)
}

func sayHello() {
    fmt.Println("Hello from goroutine!")
}
