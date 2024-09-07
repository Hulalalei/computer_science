public class Server extends Thread {

    @Override
    public void run () {
        //判断阻塞队列是否为满
        while (true) {
            synchronized (BlockingQueue.allLock) {
                //决定执行次数
                if (BlockingQueue.num <= 0) break;

                if (BlockingQueue.list.size() != BlockingQueue.capacity) {
                    //非满，生产
                    //容器中添加食物，数量更新，加锁
                    BlockingQueue.containerLock.lock();
                    BlockingQueue.list.add("食物");
                    BlockingQueue.containerLock.unlock();

                    //食物数量更改，加锁
                    BlockingQueue.foodCountLock.lock();
                    BlockingQueue.foodCount++;
                    System.out.println("食物+1");
                    BlockingQueue.foodCountLock.unlock();

                    //缓冲一下，使生产者生产均匀
                    try {
                        Server.sleep(100);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    //此时如果容器为空，生产后可唤醒消费者
                    BlockingQueue.allLock.notifyAll();
                }else {
                    //已经满了，等待
                    try {
                        //System.out.println(Server.currentThread().getName() + " Server waiting");
                        BlockingQueue.allLock.wait();//此处阻塞，交锁
                        //System.out.println(Server.currentThread().getName() + " Server awaked");
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }//else
                //System.out.println("num = " + BlockingQueue.num);
            }
        }//while
    }//run
}//class
