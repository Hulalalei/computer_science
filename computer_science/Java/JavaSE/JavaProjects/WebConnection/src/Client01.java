import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class Client01 {
    public static void main(String[] args) throws IOException, InterruptedException {
        //创建socket对象，连接服务器端，服务器ip与端口
        Socket socket = new Socket("192.168.88.130", 11068);
        System.out.println("ok");
        runs(socket);
    }

    public static void runs(Socket spy) throws IOException {

        while (true) {
            //写出数据
            System.out.println("输入：");
            Scanner sc = new Scanner(System.in);
            String name = sc.nextLine();

            //获取输出流
            //需要刷新缓冲区，否则读不出来数据
            BufferedWriter bfw = new BufferedWriter(new OutputStreamWriter(spy.getOutputStream()));

            bfw.write(name);
            bfw.newLine();
            bfw.flush();
        }
    }
}
