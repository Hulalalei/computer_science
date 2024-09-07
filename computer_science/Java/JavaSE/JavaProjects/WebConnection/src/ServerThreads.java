import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

public class ServerThreads {
    public static void main(String[] args) throws IOException {
        //创建socket对象，并绑定端口
        ServerSocket server = new ServerSocket(8000);

        //循环监听
        while (true) {
            //客户端监听
            Socket spy = server.accept();

            //启动线程
            MyThread mt = new MyThread(spy);
            mt.start();
        }

        //server.close();
    }
}
