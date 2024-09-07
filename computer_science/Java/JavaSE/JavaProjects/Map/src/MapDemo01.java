import java.util.*;
import java.util.function.BiConsumer;
import java.util.Set;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.stream.Collectors;

public class MapDemo01 {
    public static void main(String[] args) {
        //Map is an interface
        Map<Integer, String> map = new HashMap<>();

        //add the elements
        map.put(1, "hsf");
        map.put(2, "gsy");
        map.put(3, "lh");

        //reverse the map
        Set<Map.Entry<Integer, String>> entries = map.entrySet();
        //method one
        for (Map.Entry<Integer, String> pair : entries) {
            System.out.println(pair.getKey() + " - " + pair.getValue());
        }
        //method two
        map.forEach (new BiConsumer<Integer, String>() {
            @Override
            public void accept(Integer key, String value) {
                //底层逐个遍历后执行这个函数
                System.out.println(key + " - " + value);
            }
        });
        //method three
        Iterator<Map.Entry<Integer, String>> iterator = entries.iterator();
        while (iterator.hasNext()) {
            System.out.println(iterator.next());
        }


    }

    public static void test() {
        HashMap<String, Integer> hm = new HashMap<>();
        Integer[] arr = {2, 4, 1, 3, 5};
        Arrays.sort(arr, new Comparator<Integer>() {
            @Override
            public int compare(Integer o1, Integer o2) {
                //如果大于零，底层则交换数据，等价于排序
                return 0;
            }
        });
        //创建对象的时候还没有创建数组，只有在加入元素后才会创建数组
        //put中有关于比较器的底层逻辑，只有TreeMap有排序
        //TreeMap->put->fixAfterInsertion(红黑树规则)->rotate平衡树调整
        TreeMap<Integer, String> tm = new TreeMap<>(new Comparator<Integer>() {
            @Override
            public int compare(Integer o1, Integer o2) {
                return 0;
            }
        });

        //Set<String> ss = Set.of("张三");
        ArrayList<String> as = new ArrayList<>();
        as.stream().collect(Collectors.toList());
        as.toArray();

        //stream end method
        TreeMap<String, Integer> treemap = new TreeMap<String, Integer>();
        Set<Map.Entry<String, Integer>> entry = treemap.entrySet();
    }
}
