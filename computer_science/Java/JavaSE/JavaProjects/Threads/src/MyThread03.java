public class MyThread03 extends Thread {
    static int count = 1;

    @Override
    public void run() {
        while (true) {
            synchronized (MyThread03.class) {
                if (count > 100) break;
                if (count % 2 == 1) {
                    System.out.println(MyThread03.currentThread().getName() + count);
                    try {
                        MyThread03.sleep(10);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                count++;
            }
        }
    }
}
