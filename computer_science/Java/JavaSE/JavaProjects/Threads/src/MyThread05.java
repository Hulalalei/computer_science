import java.util.ArrayList;
import java.util.Collections;

public class MyThread05 extends Thread {
    //全局奖池
    static ArrayList<Integer> list = new ArrayList<>();
    static {
//        list.add(10);   list.add(5);    list.add(20);
//        list.add(50);   list.add(100);  list.add(200);
//        list.add(500);  list.add(800);  list.add(2);
//        list.add(80);   list.add(300);  list.add(700);
        Collections.addAll(list,10, 5, 20, 50, 100, 200, 500, 800, 2, 80, 300, 700);
    }

    @Override
    public void run () {
        while (true) {
            synchronized (MyThread05.class) {
                if (list.isEmpty()) {
                    System.out.println("奖池已抽完");
                    break;
                }else {
                    Collections.shuffle(list);
                    System.out.println(MyThread05.currentThread().getName() +
                            "又产生了一个" + list.get(0) + "元大奖");
                    list.remove(0);
                    try {
                        MyThread05.sleep(10);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }
}
