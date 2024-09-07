public class SCDemo {
    public static void main(String[] args) throws InterruptedException {
        Server server01 = new Server();
        Client client01 = new Client();
        Server server02 = new Server();
        Client client02 = new Client();

        //server01.setName("P ");
        //client02.setName("C ");

        client01.start();
        client02.start();
        //client02.join();//bug01: 死锁，因为必须在这个线程执行完后才会执行其他线程
        server01.start();
        server02.start();
        //bug02: 还是存在死锁，所以没打印完；不是的，苏醒得循环一次，执行又得循环一次
        //bug03: 为啥死锁，因为Client执行完了但Server还在等待中啊……
        // 苏醒后还需要一次循环才能执行啊……
    }
}
