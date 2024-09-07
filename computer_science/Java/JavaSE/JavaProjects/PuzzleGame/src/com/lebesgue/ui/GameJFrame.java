package com.lebesgue.ui;

import javax.swing.*;
import javax.swing.border.BevelBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.Random;

public class GameJFrame extends JFrame implements KeyListener, ActionListener {

    //记录图片位置
    public int[][] data = new int[3][3];

    //记录空白区域位置
    int x = 0;
    int y = 0;

    //判断游戏结束标志
    int[][] win = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 0}
    };

    //记录游戏步数
    int step = 0;

    //构造函数
    public GameJFrame() {
        //初始化界面
        InitJFrame();

        //初始化菜单
        InitJMenuBar();

        //初始化图片位置数据
        InitData();

        //初始化随机图片
        InitImage();

        //界面显示设置，写在最后
        this.setVisible(true);
    }

    private void InitData() {
        int[] arr = {0, 1, 2, 3, 4, 5, 6, 7, 8};
        //打乱一维数组
        Random r = new Random();
        for (int i = 0; i < arr.length; i++) {
            int index = r.nextInt(arr.length);
            int temp = arr[i];
            arr[i] = arr[index];
            arr[index] = temp;
        }

        //将一维数组放入二维数组中
        for (int i = 0; i < 9; i++) {
            if (arr[i] == 0) {
                x = i / 3;
                y = i % 3;
            }
            data[i / 3][i % 3]  = arr[i];
        }
    }

    public void InitImage() {
        //清空原本已经出现的图片
        this.getContentPane().removeAll();

        if (victory()) {
            JLabel winJLabel = new JLabel(new ImageIcon("D:\\Java\\JavaSE\\JavaProjects\\PuzzleGame\\Images\\win01.jpg"));
            winJLabel.setBounds(203, 283, 200, 200);
            this.getContentPane().add(winJLabel);
        }

        //step
        JLabel stepCount = new JLabel("步数：" + step);
        stepCount.setBounds(50, 30, 100, 20);
        this.getContentPane().add(stepCount);

        //载入九宫格图片
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int num = data[i][j];
                //创建一个图片对象D:\Java\JavaSE\JavaProjects\PuzzleGame\Images\01.jpg
                ImageIcon icon = new ImageIcon("D:\\Java\\JavaSE\\JavaProjects\\PuzzleGame\\Images\\0"+num+"s.jpg");
                //创建管理容器
                JLabel jLabel = new JLabel(icon);
                //设置位置
                jLabel.setBounds(200 * j + 150, 200 * i + 20, 200, 200);

                //调整图片边框
                jLabel.setBorder(new BevelBorder(BevelBorder.LOWERED));

                //将容器加到界面中
                //this.add(jLabel);
                this.getContentPane().add(jLabel);
            }
        }

        //加入背景图
        JLabel background = new JLabel(new ImageIcon("D:\\Java\\JavaSE\\JavaProjects\\PuzzleGame\\Images\\龙猫02.jpg"));
        background.setBounds(-20, -200, 1000, 1000);
        this.getContentPane().add(background);

        //刷新界面
        this.getContentPane().repaint();
    }

    public void InitJMenuBar() {
        //创建菜单
        JMenuBar jMenuBar = new JMenuBar();

        //创建选项
        JMenu functionMenu = new JMenu("功能");
        JMenu aboutMenu = new JMenu("关于我们");

        //创建选项下拉框
        JMenuItem replayItem = new JMenuItem("重新开始");
        JMenuItem reLoginItem = new JMenuItem("重新登录");
        JMenuItem closeItem = new JMenuItem("关闭游戏");

        JMenuItem accountItem = new JMenuItem("公众号");

        //建立以上三个层次之间的联系
        functionMenu.add(replayItem);
        functionMenu.add(reLoginItem);
        functionMenu.add(closeItem);

        aboutMenu.add(accountItem);

        jMenuBar.add(functionMenu);
        jMenuBar.add(aboutMenu);

        //将菜单放入界面框中
        this.setJMenuBar(jMenuBar);
    }

    public void InitJFrame() {
        //宽高
        this.setSize(900, 700);
        //标题
        this.setTitle("拼图 v1.0");
        //保持在界面顶端
        this.setAlwaysOnTop(true);
        //居中
        this.setLocationRelativeTo(null);
        //关闭界面设置
        this.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        //取消默认的居中放置，只有取消了才会按XY轴的形式添加组件
        this.setLayout(null);

        //设置监听
        this.addKeyListener(this);
    }

    @Override
    public void keyTyped(KeyEvent e) {

    }

    //设置shift为快捷键，查看完整图片
    @Override
    public void keyPressed(KeyEvent e) {
        int code = e.getKeyCode();

        //按住时显示完整图片
        if (code == 16) {//shift
            //先删除所有图片
            this.getContentPane().removeAll();

            //加入完整图
            JLabel whole = new JLabel(new ImageIcon("D:\\Java\\JavaSE\\JavaProjects\\PuzzleGame\\Images\\all.jpg"));
            whole.setBounds(150, 20, 600, 600);
            this.getContentPane().add(whole);

            //加入背景图
            JLabel background = new JLabel(new ImageIcon("D:\\Java\\JavaSE\\JavaProjects\\PuzzleGame\\Images\\龙猫02.jpg"));
            background.setBounds(-20, -200, 1000, 1000);
            this.getContentPane().add(background);

            //刷新界面
            this.repaint();
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        //判断结束
        if (victory()) {
            return;
        }

        int code = e.getKeyCode();

        //松开后变回原样
        if (code == 16) {//shift
            this.InitImage();
        }

        //一键通关
        if (code == 17) {//ctrl
            data = new int[][]{
                {1, 2, 3},
                {4, 5, 6},
                {7, 8, 0}
            };

            this.InitImage();
        }

        System.out.println(code);
        if (code == 65) {
            //左
            if (y == 3) {//数组溢出判断
                return;
            }
            data[x][y] = data[x][y + 1];
            data[x][y + 1] = 0;
            y++;
            step++;
            InitImage();
        }
        else if (code == 87) {
            //上
            if (x == 3) {//数组溢出判断
                return;
            }
            data[x][y] = data[x + 1][y];
            data[x + 1][y] = 0;
            x++;
            step++;
            InitImage();
        }
        else if (code == 68) {
            //右
            if (y == 0) {//数组溢出判断
                return;
            }
            data[x][y] = data[x][y - 1];
            data[x][y - 1] = 0;
            y--;
            step++;
            InitImage();
        }
        else if (code == 83) {
            //下
            if (x == 0) {//数组溢出判断
                return;
            }
            data[x][y] = data[x - 1][y];
            data[x - 1][y] = 0;
            x--;
            step++;
            InitImage();
        }
    }

    //判断是否完成游戏
    public boolean victory() {
        for (int i = 0; i < data.length; i++) {
            for (int j = 0; j < data[i].length; j++) {
                if (data[i][j] != win[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    @Override
    public void actionPerformed(ActionEvent e) {

    }
}
