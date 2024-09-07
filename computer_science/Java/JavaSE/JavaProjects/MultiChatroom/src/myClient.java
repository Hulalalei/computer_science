import java.io.*;
import java.net.Socket;
import java.util.HashMap;
import java.util.Scanner;

public class myClient {
    public static void main(String[] args) throws IOException {

        //创建socket对象，用于验证登录正确性
        Socket socket01 = new Socket("127.0.0.1", 8000);
        //创建socket对象，用于注册
        Socket socket02 = new Socket("127.0.0.1", 8060);
        //创建socket对象，用于聊天
        Socket socket03 = new Socket("127.0.0.1", 8120);


        //进入对应接口
        while (true) {
            //界面
            System.out.println("服务器连接成功...");
            System.out.println("===欢迎来到hsf发起的聊天室===");
            System.out.println("========1、登录========");
            System.out.println("========2、注册========");
            System.out.println("========3、退出========");
            System.out.println("终端：");
            Scanner sc = new Scanner(System.in);
            String option = sc.nextLine();

            switch (option) {
                case "1": {
                    Login(socket01, socket03);
                    break;
                }
                case "2": {
                    Register(socket02);
                    break;
                }
                case "3": {
                    System.exit(0);
                    break;
                }
                default: {
                    System.out.println("重新输入，呆子");
                    option = sc.nextLine();
                    break;
                }
            }
        }
    }

    //登录验证界面
    public static void Login(Socket socket01, Socket socket03) throws IOException {
        //客户端获取客户信息
        Scanner sc = new Scanner(System.in);
        System.out.println("用户名：");
        String name = sc.nextLine();
        System.out.println("密码：");
        String password = sc.nextLine();

        //发送到服务器端
        BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(socket01.getOutputStream()));//缓冲流
        bw.write(name + "=" + password);
        bw.newLine();
        bw.flush();

        //接收服务器的响应，判断是否登录成功
        BufferedReader bfr = new BufferedReader(new InputStreamReader(socket01.getInputStream()));
        String yesOrNot = "";
        while (true) {
            yesOrNot = bfr.readLine();
            //既然循环出不来，那就不出来了，直接在里面执行
            if ("yes".equals(yesOrNot)) {
                GetInRoom(socket03);
            }else if ("no".equals(yesOrNot)){
                System.out.println("重新输入");
                break;
            }
        }
    }

    //注册界面
    public static void Register (Socket socket02) throws IOException {
        //获取信息
        Scanner sc = new Scanner(System.in);
        System.out.println("用户名：");
        String name = sc.nextLine();
        System.out.println("密码：");
        String password = sc.nextLine();
        String infos = name + "=" + password;

        //信息传到服务器，那边添加信息
        BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(socket02.getOutputStream()));
        bw.write(infos);
        bw.newLine();
        bw.flush();
    }

    //加入聊天室
    public static void GetInRoom (Socket socket03) throws IOException {
        //加载
        System.out.println("Login in successfully!");
        System.out.println("welcome to chat room!");
        System.out.println("you can speak now");
        //获取输出流
        BufferedWriter bwr = new BufferedWriter(new OutputStreamWriter(socket03.getOutputStream()));
        //持续聊天
        while (true) {
            Scanner sc = new Scanner(System.in);
            String things = sc.nextLine();
            if ("886".equals(things)) {
                System.exit(0);
            }
            bwr.write(things);
            bwr.newLine();
            bwr.flush();
        }
    }
}
