package com.lebesgue.test01;

public class Dog extends Animal implements Swim{
    public Dog() {
    }

    public Dog(int age, String color) {
        super(age, color);
    }

    @Override
    public void eat(String something) {
        System.out.println("it is eating " + something + " happily with its front feet grabing the bowel.");
    }
    public void lookHome() {
        System.out.println("the dog can guard the home");
    }

    @Override
    public void swim() {
        System.out.println("the dog can swim");
    }
}
