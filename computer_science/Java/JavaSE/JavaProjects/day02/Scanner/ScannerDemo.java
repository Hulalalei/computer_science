import java.util.Scanner;

public class ScannerDemo {
	public static void main(String[] args){
		//创建 键盘录入 对象并初始化
		Scanner sc = new Scanner(System.in);
		
		//调用成员函数
		int i = sc.nextInt();

		System.out.println(i);
	}

}