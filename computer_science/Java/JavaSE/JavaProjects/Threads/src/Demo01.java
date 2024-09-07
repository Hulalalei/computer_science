public class Demo01 {
    public static void main(String[] args) {
        //创建线程
        MyThread01 mt01 = new MyThread01();
        MyThread02 mt02 = new MyThread02();

        //启动线程
        mt01.start();
        mt02.start();
    }
}
