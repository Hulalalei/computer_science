import java.io.*;
import java.net.Socket;
import java.net.SocketException;
import java.util.HashMap;

//先接收到客户端的数据后再读档，不然无法同步信息
public class ServerThreadsLogin extends Thread {
    public static HashMap<String, String> infos = new HashMap<>();
    public Socket spy;
    ServerThreadsLogin(Socket spy) {
        this.spy = spy;
    }

    @Override
    public void run () {
        //客户端流
        BufferedReader clientInfo = null;
        try {
            clientInfo = new BufferedReader(new InputStreamReader(spy.getInputStream()));
        } catch (IOException e) {
            e.printStackTrace();
        }

        //服务器发送信息流
        BufferedWriter bfw = null;
        try {
            bfw = new BufferedWriter(new OutputStreamWriter(spy.getOutputStream()));
        } catch (IOException e) {
            e.printStackTrace();
        }

        //读取文档流
        BufferedReader getInfo = null;
        try {
            getInfo = new BufferedReader(new FileReader("D:\\Java\\JavaSE\\JavaProjects\\MultiChatroom\\readMe.txt"));
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        //temp02读取客户端流
        String temp02 = "";
        String[] split02 = null;
        try {
            temp02 = clientInfo.readLine();//bug01
            System.out.println(temp02);
            split02 = temp02.split("=");
        } catch (IOException e) {
            e.printStackTrace();
        }

        while (true) {
            //读取文档流，放入容器中
            String[] split01;
            while (true) {
                String temp01 = "";
                try {
                    temp01 = getInfo.readLine();
                    if (temp01 == null) break;
                } catch (IOException e) {
                    e.printStackTrace();
                }
                split01 = temp01.split("=");
                infos.put(split01[0], split01[1]);
            }

            //判断key //获取value
            if (infos.containsKey(split02[0]) && infos.get(split02[0]).equals(split02[1])) {
                try {
                    bfw.write("yes");
                    bfw.newLine();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }else {//就一次循环，else不需要扔外面
                try {
                    bfw.write("no");
                    bfw.newLine();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
