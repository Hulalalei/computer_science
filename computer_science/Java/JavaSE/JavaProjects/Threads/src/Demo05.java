public class Demo05 {
    public static void main(String[] args) {
        MyThread05 mt01 = new MyThread05();
        MyThread05 mt02 = new MyThread05();

        mt01.setName("奖池一");
        mt02.setName("奖池二");

        mt01.start();
        mt02.start();
    }
}
