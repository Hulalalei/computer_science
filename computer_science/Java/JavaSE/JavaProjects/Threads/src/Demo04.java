public class Demo04 {
    public static void main(String[] args) {
        MyThread04 mt01 = new MyThread04();
        MyThread04 mt02 = new MyThread04();
        MyThread04 mt03 = new MyThread04();
        MyThread04 mt04 = new MyThread04();
        MyThread04 mt05 = new MyThread04();

        mt01.setName("A");
        mt02.setName("B");
        mt03.setName("C");
        mt04.setName("D");
        mt05.setName("E");

        mt01.start();
        mt02.start();
        mt03.start();
        mt04.start();
        mt05.start();
    }
}
