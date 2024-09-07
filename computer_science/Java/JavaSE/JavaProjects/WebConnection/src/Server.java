import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {
    public static void main(String[] args) throws IOException {
        //创建socket对象，并绑定端口
        ServerSocket server01 = new ServerSocket(8000);
        //客户端监听
        Socket spy01 = server01.accept();

        //获取监听到的流
        BufferedReader bfr = new BufferedReader(new InputStreamReader(spy01.getInputStream()));


        String things = "";
        things = bfr.readLine();
        System.out.println(things);
    }
}
