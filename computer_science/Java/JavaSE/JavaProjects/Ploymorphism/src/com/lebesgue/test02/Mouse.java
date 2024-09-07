package com.lebesgue.test02;

import com.lebesgue.test01.Animal;

public class Mouse extends Animal {
    public Mouse() {
    }

    public Mouse(int age, String color) {
        super(age, color);
    }

    @Override
    public void eat(String something) {
        this.doingJob();
    }

}
