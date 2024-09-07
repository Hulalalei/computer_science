import java.io.*;

public class Demo01 {
    public static void main(String[] args) throws IOException {
        //拷贝文件夹，涉及子文件
        //创建源文件和目的文件对象
        File source = new File("D:\\Java\\JavaSE\\fileTest");
        File dest = new File("D:\\Java\\JavaSE\\IOTest");

        //进行拷贝操作
        copyDir(source, dest);
    }

    public static void copyDir(File source, File dest) throws IOException {
        //创建目标文件夹，存在则创建失败
        dest.mkdir();

        //获取文件夹内容
        File[] files = source.listFiles();

        //遍历
        for (File file : files) {
            if (file.isFile()) {
                //是文件，字节流
                //读取
                FileInputStream fis = new FileInputStream(file);

                //获取文件名并创建
                String start = dest.getAbsolutePath();
                String end = dest.getAbsolutePath() + "\\" + file.getName();
                dest = new File(end);

                //写入，文件不存在，报错
                FileOutputStream fos = new FileOutputStream(dest);

                //设置缓冲区
                byte[] bytes = new byte[1024 * 1024 *5];
                int len = 0;
                //循环读取并写入
                while((len = fis.read(bytes)) != -1) {
                    fos.write(bytes, 0, len);
                }

                //不是文件夹，路径得再改回去才能继续添加
                dest = new File(start);

                //复制完后关流
                fos.close();
                fis.close();
            }else {
                //不是文件，dest里创建文件夹
                String end = dest.getAbsolutePath() + "\\" + file.getName();
                dest = new File(end);
                dest.mkdir();
                copyDir(file, dest);
            }
        }
    }
}
