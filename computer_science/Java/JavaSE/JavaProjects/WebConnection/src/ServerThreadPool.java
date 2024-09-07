import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class ServerThreadPool {
    public static void main(String[] args) throws IOException {
        //创建线程池对象
        ThreadPoolExecutor pool =new ThreadPoolExecutor(
          3,
          16,
          60,
           TimeUnit.SECONDS,
           new ArrayBlockingQueue<>(2),
           Executors.defaultThreadFactory(),
           new ThreadPoolExecutor.AbortPolicy()
        );

        //创建socket对象，并绑定端口
        ServerSocket server = new ServerSocket(8000);

        //循环监听
        while (true) {
            //客户端监听
            Socket spy = server.accept();

            pool.submit(new MyThread(spy));
        }

        //server.close();
    }
}
