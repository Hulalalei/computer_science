import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class Client02 {
    public static void main(String[] args) throws IOException, InterruptedException {
        //创建socket对象，连接服务器端
        Socket socket = new Socket("127.0.0.1", 8000);
        //获取输出流
        OutputStream ops = socket.getOutputStream();
        OutputStreamWriter osw = new OutputStreamWriter(ops);
        //需要刷新缓冲区，否则读不出来数据
        BufferedWriter bfw = new BufferedWriter(osw);
        int num = 10;
        while (true) {
            //写出数据
            bfw.write("hello guys!02");
            bfw.newLine();
            bfw.flush();
            Thread.sleep(1000);
            num--;
            if (num == 0) {
                break;
            }
        }

        //关通信，因为是死循环，所以跳不出来而报错
        socket.close();
    }
}
