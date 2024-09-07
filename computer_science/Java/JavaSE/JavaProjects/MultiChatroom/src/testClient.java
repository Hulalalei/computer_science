import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class testClient {
    public static void main(String[] args) throws IOException {
        //创建socket对象，用于验证登录正确性
        Socket socket01 = new Socket("127.0.0.1", 8000);

        Login(socket01);

    }

    //登录验证界面
    public static void Login(Socket socket01) throws IOException {
        //客户端获取客户信息
        Scanner sc = new Scanner(System.in);
        System.out.println("用户名：");
        String name = sc.nextLine();
        System.out.println("密码：");
        String password = sc.nextLine();

        //发送到服务器端
        BufferedWriter bfw = new BufferedWriter(new OutputStreamWriter(socket01.getOutputStream()));//缓冲流
        bfw.write(name + "=" + password);
        bfw.newLine();
        bfw.flush();
    }
}
