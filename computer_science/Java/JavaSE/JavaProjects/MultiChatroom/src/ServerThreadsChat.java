import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class ServerThreadsChat extends Thread {
    public Socket spy;
    ServerThreadsChat(Socket socket) {
        this.spy = socket;
    }

    @Override
    public void run () {
        BufferedReader bfr = null;
        try {
            bfr = new BufferedReader(new InputStreamReader(spy.getInputStream()));
        } catch (IOException e) {
            e.printStackTrace();
        }
        String temp = "";
        while (true) {
            try {
                if (!((temp = bfr.readLine()) != null)) break;
            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.println(temp);
        }
    }
}
