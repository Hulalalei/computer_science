package com.lebesgue.test01;

public class Person {
    private String name;
    private int age;

    public Person() {
    }

    public Person(String name, int age) {
        this.name = name;
        this.age = age;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }

    public void keepPet(Dog dog, String something) {
        System.out.println(this.getName() + " who is already " + this.getAge() +
                " kept a " + dog.getColor() + " skin dog of " + dog.getAge());
        dog.eat(something);
    }
    public void keepPet(Cat cat, String something) {
        System.out.println(this.getName() + " who is already " + this.getAge() +
                " kept a " + cat.getColor() + " skin cat of " + cat.getAge());
        cat.eat(something);
    }
    public void keepPet(Animal ani, String something) {
        if (ani instanceof Dog) {
            Dog d = (Dog)ani;
            d.lookHome();
        }
        else if (ani instanceof Cat) {
            Cat c = (Cat)ani;
            c.catchMouse();
        }
        System.out.println(this.getName() + " who is already " + this.getAge() +
                " kept a " + ani.getColor() + " skin cat of " + ani.getAge());
        ani.eat(something);
    }
}
