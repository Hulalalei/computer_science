public class basketPlayer extends Person {
    public basketPlayer() {
    }

    public basketPlayer(String name, int age) {
        super(name, age);
    }

    @Override
    public void doJob() {
        System.out.println("learning how to play basketball");
    }
}
