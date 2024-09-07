package com.lebesgue.test01;

public class Test01 {
    public static void main(String[] args) {
        Person w = new Person("w",30);
        Person li = new Person("li", 25);

        Dog ani01 = new Dog();
        ani01.setAge(3);
        ani01.setColor("black");

        Cat ani02 = new Cat();
        ani02.setAge(2);
        ani02.setColor("gray");

        //构建一个可以接收各种子类对象的方法，此方法参数为父类对象
        w.keepPet(ani01, "bone");
        li.keepPet(ani02, "fish");
        ani01.swim();
    }
}
