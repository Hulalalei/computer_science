public class basketCoach extends Person {
    public basketCoach() {
    }

    public basketCoach(String name, int age) {
        super(name, age);
    }

    @Override
    public void doJob() {
        System.out.println("teach students how to play it");
    }
}
