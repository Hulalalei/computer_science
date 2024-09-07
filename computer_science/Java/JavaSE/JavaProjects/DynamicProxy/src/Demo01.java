public class Demo01 {
    public static void main(String[] args) {
        Star star = ProxyUtil.createProxy(new BigStar("鸡哥"));
        star.dance();
        star.sing("基你态美");
    }
}
