import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.*;

public class myServer {
    public static void main(String[] args) throws IOException {
        //创建线程池对象
        ThreadPoolExecutor chatPool01 = new ThreadPoolExecutor(
                3,
                16,
                60,
                TimeUnit.SECONDS,
                new ArrayBlockingQueue<>(3),
                Executors.defaultThreadFactory(),
                new ThreadPoolExecutor.AbortPolicy()
        );
        ThreadPoolExecutor chatPool02 = new ThreadPoolExecutor(
                3,
                16,
                60,
                TimeUnit.SECONDS,
                new ArrayBlockingQueue<>(3),
                Executors.defaultThreadFactory(),
                new ThreadPoolExecutor.AbortPolicy()
        );
        ThreadPoolExecutor chatPool03 = new ThreadPoolExecutor(
                3,
                16,
                60,
                TimeUnit.SECONDS,
                new ArrayBlockingQueue<>(3),
                Executors.defaultThreadFactory(),
                new ThreadPoolExecutor.AbortPolicy()
        );

        //创建ServerSocket对象
        ServerSocket server01 = new ServerSocket(8000);//登录
        ServerSocket server02 = new ServerSocket(8060);//注册
        ServerSocket server03 = new ServerSocket(8120);//聊天

        //服务器持续运行
        while (true) {
            //持续监听
            Socket spy01 = server01.accept();
            Socket spy02 = server02.accept();
            Socket spy03 = server03.accept();

            //调用线程池
            chatPool01.submit(new ServerThreadsLogin(spy01));
            chatPool03.submit(new ServerThreadsRegister(spy02));
            chatPool02.submit(new ServerThreadsChat(spy03));
        }
    }
}
