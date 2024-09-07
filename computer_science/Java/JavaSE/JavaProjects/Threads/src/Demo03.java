public class Demo03 {
    public static void main(String[] args) {
        MyThread03 mt01 = new MyThread03();
        MyThread03 mt02 = new MyThread03();

        mt01.setName("A");
        mt02.setName("B");

        mt01.start();
        mt02.start();
    }
}
