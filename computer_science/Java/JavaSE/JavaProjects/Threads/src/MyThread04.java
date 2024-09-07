import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Random;

public class MyThread04 extends Thread {
    //红包金额
//    static int bag01 = new Random().nextInt(50);
//    static int bag02 = new Random().nextInt(40);
//    static int bag03 = 100 - bag01 - bag02;

    //红包数目
    static int bagCount = 3;
    //总金额
    static int money = 100;

    @Override
    public void run () {
        //while (true) {
            synchronized (MyThread04.class) {
                if (bagCount == 0) {
                    System.out.println("红包已抢完, " + MyThread03.currentThread().getName() + "没抢到红包");
                    //break;
                }else {
                    int temp = new Random().nextInt(40);
                    if (bagCount == 1) {
                        temp = money;
                        System.out.println(MyThread03.currentThread().getName() + "抢到红包：" + temp);
                    } else {
                        money = money - temp;
                        System.out.println(MyThread03.currentThread().getName() + "抢到红包：" + temp);
                        try {
                            MyThread04.sleep(1000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                    bagCount--;
                }
            }
        //}//while
    }
}
