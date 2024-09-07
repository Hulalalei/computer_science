public class tennisPlayer extends Person implements speakEnglish {
    public tennisPlayer() {
    }

    public tennisPlayer(String name, int age) {
        super(name, age);
    }

    @Override
    public void doJob() {
        System.out.println("learning how to play table tennis");
    }

    @Override
    public void speaking() {
        System.out.println("students can speak English very well");
    }
}
