import java.util.ArrayList;
import java.util.Random;
import java.util.Scanner;


public class StuSysPlus {
    //提高代码可读性，final修饰静态变量
    private static final String LOGIN = "1";
    private static final String REGISTER = "2";
    private static final String FORGET_PASSWORD = "3";

    //初始化容器
    //两容器均为地址引用，可作为形参
    //存储用户记录表
    static ArrayList<Users> container = new ArrayList<>();
    //存储用户名表
    static ArrayList<String> Uname = new ArrayList<>();

    //静态代码块
    static {
        container.add(new Users("hsf", "123123", "123123123123123123", "12312312311"));
        Uname.add(new String("hsf"));
    }
    
    public static void main(String[] args) {
        //功能实现
        while (true) {
            System.out.println("欢迎来到学生管理系统");
            System.out.println("请选择操作：1、登录 2、注册 3、忘记密码");

            Scanner sc = new Scanner(System.in);
            String choose = sc.next();
            switch (choose) {
                case LOGIN: {
                    Login(container);
                    break;
                }
                case REGISTER: {
                    Register(container, Uname);
                    break;
                }
                case FORGET_PASSWORD: {
                    ForgetPassword(container);
                    break;
                }
            }
        }
    }

    public static void Login(ArrayList<Users> container) {
        while(true) {
            if(!isEmpty(container)) {
                for (int i = 0; i < 3; i++) {
                    //创建对象存储用户信息
                    Users user = new Users();
                    Scanner sc = new Scanner(System.in);
                    System.out.println("请输入用户名");
                    user.setUserName(sc.next());
                    System.out.println("请输入密码");
                    user.setPassWord(sc.next());

                    //判断是否登录成功
                    if(isCorrect(container, user)) {
                        String VeriCode = getVeriNum();
                        System.out.println("请输入验证码，验证码为： " + VeriCode);
                        String veri = sc.next();
                        if (veri.equals(VeriCode)) {
                            System.out.println("登录成功！");
                            return;
                        }
                        else {
                            System.out.println("验证码验证失败");
                        }
                    }
                    else {
                        int num = 3 - i;
                        System.out.println("登录失败，请重新尝试，剩余尝试次数为" + num + "次，次数用完后程序将停止运行");
                    }
                }//for
                System.exit(0);
            }//if
            else {
                System.out.println("当前暂无账号可用，请先注册");
                return;
            }
        }
    }
    public static void Register(ArrayList<Users> container, ArrayList<String> Uname) {
        while (true) {
            //创建对象存储用户信息
            Users user = new Users();
            Scanner sc = new Scanner(System.in);

            //判断用户名是否唯一
            System.out.println("请输入用户名");
            String name = sc.next();
            user.setUserName(name);
            //用户名验证
            if (isUnique(Uname, user.getUserName()) && isOK(user.getUserName())) {
                Uname.add(name);
                //密码验证
                loop: while (true) {
                    //两次密码循环标志
                    int flag = 2;
                    String pass1 = "1", pass2 = "0";
                    //密码验证
                    for (; flag > 0; flag--) {
                        System.out.println("请输入密码");
                        if (flag == 2) {
                            pass1 = sc.next();
                        }
                        else if (flag == 1) {
                            pass2 = sc.next();
                        }
                    }
                    //验证成功
                    if (pass1.equals(pass2)) {//String为引用，比较的是地址
                        user.setPassWord(pass1);
                        System.out.println("ok");
                        break loop;
                    }
                    else {
                        System.out.println("两次密码不一致，请重新输入");
                    }
                }

                //身份证验证
                while (true) {
                    System.out.println("请输入身份证号码");
                    String id = sc.next();
                    if (isIdOK(id)) {
                        user.setIdCard(id);
                        break;
                    }
                    else {
                        System.out.println("不是身份证号格式，请重新输入");
                    }
                }

                //手机号验证
                while (true) {
                    System.out.println("请输入手机号");
                    String num = sc.next();
                    if (isNumOK(num)) {
                        user.setPhoneNum(num);
                        break;
                    }
                    else {
                        System.out.println("不是手机号，请重新输入");
                    }
                }

                container.add(user);
                System.out.println("注册成功！");
                break;
            }
            else {
                System.out.println("用户名已存在或不符合要求，请重新输入");
            }
        }
    }
    public static void ForgetPassword(ArrayList<Users> container) {
        if (!isEmpty(container)) {
            System.out.println("请输入您的身份证信息以验证");
            System.out.println("请输入用户名");
            Scanner sc = new Scanner(System.in);
            String name = sc.next();
            if (isCorrect02(container, name)) {
                int i = isCorrect03(container, name);
                System.out.println("请输入身份证号码");
                String id  = sc.next();
                System.out.println("请输入手机号码");
                String phonenum = sc.next();
                if (container.get(i).getIdCard().equals(id) &&
                        container.get(i).getPhoneNum().equals(phonenum)) {
                    System.out.println("请输入新的密码");
                    String password = sc.next();
                    container.get(i).setPassWord(password);
                    System.out.println("修改成功");
                }
                else {
                    System.out.println("验证失败，请重新验证");
                    return;
                }
            }
            else {
                System.out.println("未注册");
                return;
            }
        }
        else {
            System.out.println("未进行注册，无需找回");
        }
    }
    public static boolean isEmpty(ArrayList<Users> container) {
        if(container.size() == 0)
            return true;//空
        else
            return false;//非空
    }
    
    public static boolean isCorrect(ArrayList<Users> container, Users user) {
        for(int i = 0; i < container.size(); i++) {
            if(container.get(i).getUserName().equals(user.getUserName())
                    && container.get(i).getPassWord().equals(user.getPassWord())) {
                return true;
            }
        }
        return false;
    }
    public static boolean isCorrect02(ArrayList<Users> container, String user) {
        for(int i = 0; i < container.size(); i++) {
            if(container.get(i).getUserName().equals(user)) {
                return true;
            }
        }
        return false;
    }
    public static int isCorrect03(ArrayList<Users> container, String user) {
        for(int i = 0; i < container.size(); i++) {
            if(container.get(i).getUserName().equals(user)) {
                return i;
            }
        }
        return -1;
    }
    public static boolean isUnique(ArrayList<String> Uname, String name) {
        for (int i = 0; i < Uname.size(); i++) {
            if (Uname.get(i).equals(name)) {
                return false;
            }
        }
        return true;
    }
    public static boolean isOK(String name) {
        int number = name.length();
        if (number > 2 && number < 16) {
            int numflag = 0;
            int charflag = 0;
            //判断用户名命名是否符合规则
            for (int i = 0; i < name.length(); i++) {
                char ch = name.charAt(i);
                if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                    charflag++;
                }
                if (ch >= 48 && ch <= 57) {
                    numflag++;
                }
            }
            if (charflag == 0) {
                return false;
            }
            else {
                return true;
            }
        }
        return false;
    }
    public static boolean isIdOK(String id) {
        if (id.length() == 18 && id.charAt(0) != '0') {
            int chflag = 17;
            for (int i = 0; i < 17; i++) {
                if (id.charAt(i) >= '0' && id.charAt(i) <= '9') {
                    chflag--;
                }
            }
            if (chflag == 0) {
                if ((id.charAt(17) >= '0' && id.charAt(17) <= '9') ||
                        id.charAt(17) == 'X' || id.charAt(17) == 'x') {
                    return true;
                }
            }
        }
        return false;
    }
    public static boolean isNumOK (String num) {
        if (num.length() == 11 && num.charAt(0) != '0') {
            for (int i = 0; i < 11; i++) {
                if (num.charAt(i) <= '0' || num.charAt(i) >= '9') {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    public static String getVeriNum() {
        String veri = new String();
        //veri.concat("aaaa");
        Random rd = new Random();
        int rad = rd.nextInt(4);
        String str = "abcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < 5; i++) {
            if (i == rad) {
                //拼接数字
                int rads = rd.nextInt(9);
                veri = veri + rads;
        }
            else {
                int rads = rd.nextInt(25);
                veri = veri + str.charAt(rads);
            }
        }
        return veri;
    }
}