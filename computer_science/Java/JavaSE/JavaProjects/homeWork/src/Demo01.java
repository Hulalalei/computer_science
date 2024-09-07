import java.util.ArrayList;
import java.util.Collections;

public class Demo01 {
    public static void main(String[] args) {
        //班级有N个学生，学生属性：姓名、年龄、性别
        //实现随机点名
        Student s1 = new Student("hsf", 19, "male");
        Student s2 = new Student("gsy", 21, "male");
        Student s3 = new Student("lh", 21, "male");

        ArrayList<Student> as = new ArrayList<>();
        Collections.addAll(as, s1, s2, s3);
        Collections.shuffle(as);
        System.out.println(as.get(0));
    }
}
