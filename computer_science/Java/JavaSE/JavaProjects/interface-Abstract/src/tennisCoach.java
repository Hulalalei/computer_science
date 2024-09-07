public class tennisCoach extends Person implements speakEnglish{
    public tennisCoach() {
    }

    public tennisCoach(String name, int age) {
        super(name, age);
    }

    @Override
    public void doJob() {
        System.out.println("teach students how to paly it");
    }

    @Override
    public void speaking() {
        System.out.println("they can speak English fluently");
    }
}
