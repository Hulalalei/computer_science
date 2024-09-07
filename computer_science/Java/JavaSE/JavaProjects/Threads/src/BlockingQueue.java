import java.util.ArrayList;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class BlockingQueue {
    //全部均设置为静态变量，方便本包下其他类用此类名调用
    //容量
    public static int capacity = 3;
    //当前存储量，得加锁
    public static int foodCount = 0;
    public static Lock foodCountLock = new ReentrantLock();
    //容器，得加锁
    public static ArrayList<String> list = new ArrayList<>();
    public static Lock containerLock = new ReentrantLock();
    //生产者锁、消费者锁
    public static Object serverLock = new Object();
    public static Object clientLock = new Object();
    public static Object allLock = new Object();

    //全局，统一线程循环次数
    public static int num = 100;
}
