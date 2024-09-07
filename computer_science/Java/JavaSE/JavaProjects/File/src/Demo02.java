import java.io.File;

public class Demo02 {
    public static void main(String[] args) {
        File file = new File("E:\\SteamLibrary");

        System.out.println("高思源簧片位置为：");
        getAdultVedio(file);
        //Exception in thread "main" java.lang.NullPointerException
        //可能会访问没有权限的文件，会返回null
    }

    public static void getAdultVedio(File file) {
        File[] files = file.listFiles();

        if (files != null) {
            for (File file1 : files) {
                if (file1.isFile()) {
                    //是文件，判断
                    String name = file1.getName();
                    String[] split = name.split("\\.");
                    if (split[split.length - 1].equals("mp4")) {
                        System.out.println(file1.getAbsolutePath());
                    }
                }else {
                    //不是文件，继续查找
                    getAdultVedio(file1);
                }
            }
        }
    }
}