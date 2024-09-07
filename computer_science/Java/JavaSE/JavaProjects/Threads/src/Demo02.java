public class Demo02 {
    public static void main(String[] args) {
        MyThread02 mt01 = new MyThread02();
        MyThread02 mt02 = new MyThread02();

        mt01.setName("同学A");
        mt02.setName("同学B");

        mt01.start();
        mt02.start();
    }
}
