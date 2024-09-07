import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;

public class MyThread extends Thread {
    Socket spy;
    MyThread(Socket server) {
        this.spy = server;
    }

    @Override
    public void run () {
        //获取监听到的流
        InputStream message = null;
        try {
            message = spy.getInputStream();
        } catch (IOException e) {
            e.printStackTrace();
        }
        InputStreamReader isr = new InputStreamReader(message);
        int len = 0;
        //byte[] bytes = new byte[1024 * 5];
        while (true) {
            try {
                if (!((len = isr.read()) != -1)) break;
            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.print((char)len);
        }
        //关监听
        try {
            spy.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
