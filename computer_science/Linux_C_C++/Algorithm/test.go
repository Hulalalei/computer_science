package main

import (
	"fmt"
	"sync"
	"time"
)

// Worker 函数类型，表示工作者的工作函数
type Worker func(task interface{})

// ThreadPool 线程池结构体
type ThreadPool struct {
	workChan chan interface{} // 工作队列
	wg       sync.WaitGroup   // 等待组，用于等待所有工作者完成
	worker   Worker           // 工作函数
}

// NewThreadPool 创建一个新的线程池
func NewThreadPool(workerCount int, worker Worker) *ThreadPool {
	pool := &ThreadPool{
		workChan: make(chan interface{}),
		worker:   worker,
	}
	pool.wg.Add(workerCount)
	for i := 0; i < workerCount; i++ {
		go pool.startWorker()
	}
	return pool
}

// StartWorker 启动一个工作者goroutine
func (pool *ThreadPool) startWorker() {
	for task := range pool.workChan {
		pool.worker(task) // 执行任务
	}
	pool.wg.Done() // 任务处理完毕，标记一个工作者完成
}

// AddTask 添加一个任务到线程池
func (pool *ThreadPool) AddTask(task interface{}) {
	pool.workChan <- task
}

// Shutdown 关闭线程池
func (pool *ThreadPool) Shutdown() {
	close(pool.workChan)   // 关闭工作队列
	pool.wg.Wait()         // 等待所有工作者完成
}

func main() {
	// 创建一个有3个工作者的线程池
	pool := NewThreadPool(3, func(task interface{}) {
		fmt.Printf("处理任务: %v\n", task)
		time.Sleep(1 * time.Second) // 模拟任务处理时间
	})

	// 添加任务到线程池
	for i := 0; i < 10; i++ {
		pool.AddTask(i)
	}

	// 关闭线程池
	pool.Shutdown()
}