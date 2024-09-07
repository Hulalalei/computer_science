import java.io.*;
import java.net.Socket;
import java.util.HashMap;

public class ServerThreadsRegister extends Thread {
    public Socket spy;
    ServerThreadsRegister(Socket spy) {
        this.spy = spy;
    }

    @Override
    public void run () {
        //获取通道数据流对象
        BufferedReader br = null;
        try {
            br = new BufferedReader(new InputStreamReader(spy.getInputStream()));
        } catch (IOException e) {
            e.printStackTrace();
        }

        //写入文档数据流对象
        BufferedWriter bw = null;
        try {
            bw = new BufferedWriter(new FileWriter("D:\\Java\\JavaSE\\JavaProjects\\MultiChatroom\\readMe.txt", true));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        //接收数据并写入文档
        String temp = "";
        while (true) {
            try {
                temp = br.readLine();
            } catch (IOException e) {
                e.printStackTrace();
            }
            if (temp.equals("")) break;
            try {
                bw.newLine();
            } catch (IOException e) {
                e.printStackTrace();
            }
            try {
                bw.write(temp);
                System.out.println("添加成功");
                break;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        //关流
        try {
            bw.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
