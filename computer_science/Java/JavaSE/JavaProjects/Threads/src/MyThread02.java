public class MyThread02 extends Thread {
    static int gifts = 100;

    @Override
    public void run() {
        while (true) {
            synchronized (MyThread02.class) {
                if (gifts < 10) {
                    break;
                }
                gifts--;
                System.out.println("发送一份礼物");
                System.out.println(MyThread02.currentThread().getName() +
                        "停止发送，总的还剩" + gifts);
                try {
                    MyThread02.sleep(10);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }//while
    }//run
}
