package com.lebesgue.test01;

public class Cat extends Animal implements Swim{
    public Cat() {
    }

    public Cat(int age, String color) {
        super(age, color);
    }


    @Override
    public void eat(String something) {
        System.out.println(" it ate the " +
                something + " with squint and tilt its head.");
    }

    public void catchMouse() {
        System.out.println("the cat can catch the mouse");
    }

    @Override
    public void swim() {
        System.out.println("the cat cannot swim");
    }
}
