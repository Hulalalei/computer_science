import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

public class testServer {
    public static void main(String[] args) throws IOException {
        //创建ServerSocket对象
        ServerSocket server01 = new ServerSocket(8000);//登录

        //持续监听
        Socket spy01 = server01.accept();

        //客户端流
        BufferedReader bfr = new BufferedReader(new InputStreamReader(spy01.getInputStream()));


        //temp02读取客户端流
        String temp02 = bfr.readLine();
        System.out.println(temp02);
    }
}
