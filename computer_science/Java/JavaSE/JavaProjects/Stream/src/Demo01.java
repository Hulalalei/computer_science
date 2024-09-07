import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;
import java.util.function.Predicate;
import java.util.stream.Collectors;

public class Demo01 {
    public static void main(String[] args) {
        ArrayList<Integer> num = new ArrayList<>();
        num.add(1);
        num.add(2);
        num.add(3);
        num.add(4);
        num.add(5);
        num.add(6);
        num.add(7);
        num.add(8);
        num.add(9);
        num.add(10);

        List<Integer> collect = num.stream().filter(new Predicate<Integer>() {
            @Override
            public boolean test(Integer value) {
                return value % 2 == 0 ? true : false;
            }
        }).collect(Collectors.toList());

        System.out.println(collect);
    }
}
