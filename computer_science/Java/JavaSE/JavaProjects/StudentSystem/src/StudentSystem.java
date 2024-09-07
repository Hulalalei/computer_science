import java.util.ArrayList;
import java.util.Scanner;

public class StudentSystem {
    public static void main(String[] args) {
        ArrayList<Student> container = new ArrayList<>();

        loop: while(true) {
            System.out.println("------------欢迎来到黑马学生管理系统-------------");
            System.out.println("1:添加学生");
            System.out.println("2:删除学生");
            System.out.println("3:修改学生");
            System.out.println("4:查询学生");
            System.out.println("5:退出");
            System.out.println("请输入您的选择：");

            Scanner sc = new Scanner(System.in);
            String choose = sc.next();//键盘录入后choose接收
            switch(choose) {
                case "1": addStudent(container);
                case "2": deleteStudent(container);
                case "3": updateStudent(container);
                case "4": quaryeStudent(container);
                case "5": {
                    System.out.println("退出");
                    break loop;
                }
                default:
                    System.out.println("没有此选项");
            }
        }
    }

    //添加学生
    public static void addStudent(ArrayList<Student> container) {
        Student s = new Student(1,"hsf",18,"henau");
        container.add(s);
    }
    //删除学生
    public static void deleteStudent(ArrayList<Student> container) {
        System.out.println("删除学生");
    }
    //修改学生
    public static void updateStudent(ArrayList<Student> container) {
        System.out.println("修改学生");
    }
    //查询学生
    public static void quaryeStudent(ArrayList<Student> container) {
        System.out.println("查询学生");
    }
}
