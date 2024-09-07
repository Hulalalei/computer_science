public class MyThread01 extends Thread {
    static int count = 1000;

    @Override
    public void run() {
        //一个线程可以执行多次
        while (true) {
            synchronized (MyThread01.class) {
                if (method()) break;
            }
        }
    }

    //同步方法绑定的是本对象this，但是创建了多个线程，只能用第二种方法调用才行
    public boolean method() {
        if (count != 0) {
            //临界资源count
            count--;
            System.out.println("还剩" + count + "张票！！！");
            try {
                MyThread01.sleep(30);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }else {
            System.out.println("票已售空！");
            return true;
        }

        return false;
    }
}
