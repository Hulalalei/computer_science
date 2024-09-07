import java.util.ArrayList;
import java.util.Map;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.stream.Collectors;

public class Demo02 {
    public static void main(String[] args) {
        ArrayList<String> member = new ArrayList<>();
        member.add("zhangsan,23");
        member.add("lisi,24");
        member.add("wangwu,25");

        //first para return key
        //second para return value
        Map<String, Integer> collect =
                member.stream()
                .filter(s -> Integer.parseInt(s.split(",")[1]) >= 24)
                .collect(Collectors.toMap(s -> s.split(",")[0], s -> Integer.parseInt(s.split(",")[1])));
        System.out.println(collect);
    }
}
