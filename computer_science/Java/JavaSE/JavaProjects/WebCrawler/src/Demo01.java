import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

//爬取文本文档……
public class Demo01 {
    public static void main(String[] args) throws IOException {
        //字符串接收网站网址
        //http://58hx.cn
        String dest = new String("https://hanyu.baidu.com/s?wd=%E7%99%BE%E5%AE%B6%E5%A7%93&from=poem");

        //调用自定义爬虫函数
        //参数一：网址
        //参数二：正则表达式
        String regex = ".{4}(，|。)";
        String target = webCrawler(dest, regex);

        //读取
        //System.out.println(target);
    }

    public static String webCrawler(String dest, String regex) throws IOException {
        //存取爬到的数据
        StringBuilder sbr = new StringBuilder();

        //创建网络对象
        URL url = new URL(dest);
        //链接上此网址
        URLConnection conn = url.openConnection();
        //读取数据，字符流
        InputStreamReader isr = new InputStreamReader(conn.getInputStream());

        char[] bytes = new char[1024];
        //(len = isr.read(bytes, 0, len)) != -1
        //函数写错了，写入的时候用这个
        int len = 0;
        while ((len = isr.read(bytes)) != -1) {
            sbr.append(bytes);
        }

        //字符串接收并过滤
        String filter = sbr.toString();
        //获取规则
        Pattern pattern = Pattern.compile(regex);
        //过滤
        Matcher matcher = pattern.matcher(filter);
        while (matcher.find()) {
            String group = matcher.group();
            System.out.println(group);
        }

        //关流
        isr.close();
        return sbr.toString();
    }
}
