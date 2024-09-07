public class Test {
    public static void main(String[] args) {
        StringBuilder sbr = new StringBuilder();
        char[] byte01 = {'1', '2', '3'};
        char[] byte02 = {'4', '5', '6'};
        sbr.append(byte01).append(byte02);
        System.out.println(sbr.toString());
    }
}
