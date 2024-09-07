import java.util.ArrayList;
import java.util.Collections;

public class Demo02 {
    public static void main(String[] args) {
        //班级有N个学生，学生属性：姓名、年龄、性别
        //实现随机点名,70%概率点到男生，30%点到女生

        Student s1 = new Student("hsf", 19, "male");
        Student s2 = new Student("gsy", 21, "female");
        Student s3 = new Student("lh", 21, "female");

        ArrayList<Student> as = new ArrayList<>();

        Collections.addAll(as, s1, s2, s3);
        int index = as.size();
        for (int i = 0; i < index; i++) {
            if (as.get(i).getSex().equals("male")) {
                for (int j = 0; j < 6; j++) {
                    Collections.addAll(as, as.get(i));//6次
                }
            }
            else if (as.get(i).getSex().equals("female")) {
                for (int j = 0; j < 2; j++) {
                    Collections.addAll(as, as.get(i));//2次
                }
            }
        }
        Collections.shuffle(as);
        System.out.println(as.get(0));
    }
}
