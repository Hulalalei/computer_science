import java.io.File;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public class Demo01 {
    public static void main(String[] args) {
        //统计一个文件夹中每种文件的个数并打印(考虑子文件)
        //创建对象
        File file = new File("D:\\Java\\JavaSE\\fileTest");

        //接收map，并遍历
        HashMap<String, Integer> countMap = getCount(file);

        System.out.println(countMap);
    }

    //获取子文件夹中文件个数
    public static HashMap<String, Integer> getCount(File file) {
        //创建map用于计数
        HashMap<String, Integer> countMap = new HashMap<>();

        //遍历
        File[] arr = file.listFiles();
        for (File file1 : arr) {
            //判断
            if (file1.isFile()) {
                //是文件
                String name = file1.getName();
                String[] split = name.split("\\.");
                //排除无后缀情况
                if (split.length >= 2) {
                    String endName = split[split.length - 1];

                    //判断元素是否存在
                    Set<Map.Entry<String, Integer>> entries = countMap.entrySet();
                    //遍历
                    int flag = 0;
                    for (Map.Entry<String, Integer> entry : entries) {
                        if (entry.getKey().equals(endName)) {
                            //存在，更新map，可以覆盖
                            countMap.put(endName, entry.getValue() + 1);
                            flag++;
                        }
                    }
                    //不存在
                    if (flag == 0) {
                        countMap.put(endName, 1);
                    }
                }
            }else {
                //是文件夹
                HashMap<String, Integer> subCountMap = getCount(file1);

                //合并
                Set<Map.Entry<String, Integer>> entries = subCountMap.entrySet();
                //遍历子map
                for (Map.Entry<String, Integer> entry : entries) {
                    //判断父map是否含有此元素
                    if (countMap.containsKey(entry.getKey())) {
                        //存在
                        int count = entry.getValue() + countMap.get(entry.getKey());
                        countMap.put(entry.getKey(), count);
                    }else {
                        //不存在
                        countMap.put(entry.getKey(), entry.getValue());
                    }
                }
            }
        }

        return countMap;
    }
}
