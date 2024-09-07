public class Client extends Thread {
    @Override
    public void run () {
        while (true) {
            synchronized (BlockingQueue.allLock) {
                //决定执行次数
                if (BlockingQueue.num <= 0) break;

                //取食物
                //有，就吃
                if (BlockingQueue.foodCount != 0) {
                    System.out.println("吃吃吃！");

                    //每吃一次，阻塞队列食物自减一次，加锁
                    BlockingQueue.containerLock.lock();
                    BlockingQueue.list.remove(0);
                    BlockingQueue.containerLock.unlock();

                    //食物数量更改，加锁
                    BlockingQueue.foodCountLock.lock();
                    BlockingQueue.foodCount--;
                    BlockingQueue.foodCountLock.unlock();
                    //缓冲一下，进食者均匀调用
                    try {
                        Client.sleep(10);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    //此时容器非满，唤醒生产者
                    BlockingQueue.allLock.notifyAll();
                }else {
                    //没有，就等着
                    try {
                        //wait 会释放锁……md
                        //时间超时后会返回锁
                        //唤醒后会重新抢夺资源
                        //System.out.println(Client.currentThread().getName() + " Client waiting");
                        BlockingQueue.allLock.wait(100);
                        //System.out.println(Client.currentThread().getName() + " Client awaked");
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }//else

                BlockingQueue.num--;
            }//synchronized
        }//while
    }//run
}//class
