import java.io.*;

public class Demo02 {
    public static void main(String[] args) throws IOException {
        //对文件加密，再使用的时候再对其进行解密
        //加密原理：对原始文件中的每一个字符数据进行修改，然后将更改以后的数据存到新的文件中
        //解密原理：读取加密文件后的文件，按照加密的规则反向操作，变成原始文件

        //源地址
        File source = new File("D:\\Java\\JavaSE\\fileTest\\01.txt");

        //目的地址
        File dest = new File("D:\\Java\\JavaSE\\IOTest\\entrypt.txt");

        //加密
        encrypt(source, dest);

        //对源文件进行销毁
        //写入
        FileOutputStream fos = new FileOutputStream(source);

        //解密
        //decrypt(dest, source);
    }

    //加密
    public static void encrypt(File source, File dest) throws IOException {
        //读取
        FileInputStream fis = new FileInputStream(source);
        //写入
        FileOutputStream fos = new FileOutputStream(dest);

        int b = 0;
        //加密处理
        while ((b = fis.read()) != -1) {
            b = b + '1';
            fos.write(b);
        }

        //关流
        fos.close();
        fis.close();
    }

    //解密
    public static void decrypt(File dest, File source) throws IOException {
        //读取
        FileInputStream fis = new FileInputStream(dest);
        //写入
        FileOutputStream fos = new FileOutputStream(source);

        int b = 0;
        //加密处理
        while ((b = fis.read()) != -1) {
            b = b - '1';
            fos.write(b);
        }

        //关流
        fos.close();
        fis.close();
    }
}
