package com.lebesgue.ui;

import javax.swing.*;

public class LoginJFrame extends JFrame {
    public LoginJFrame() {
        //宽高
        this.setSize(488, 430);
        //标题
        this.setTitle("拼图 登录");
        //保持在界面顶端
        this.setAlwaysOnTop(true);
        //居中
        this.setLocationRelativeTo(null);
        //关闭界面设置
        this.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);



        //界面显示设置，写在最后
        this.setVisible(true);
    }
}
