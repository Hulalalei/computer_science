import java.util.Objects;

public class BigStar implements Star{
    private String name;

    public BigStar() {
    }

    public BigStar(String name) {
        this.name = name;
    }

    //唱
    @Override
    public String sing (String name) {
        System.out.println(this.name + "正在唱" + name);
        return "谢谢";
    }
    //跳
    @Override
    public void dance () {
        System.out.println(this.name + "正在跳");
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    @Override
    public String toString() {
        return "BigStar{" +
                "name='" + name + '\'' +
                '}';
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        BigStar bigStar = (BigStar) o;
        return Objects.equals(name, bigStar.name);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name);
    }
}
