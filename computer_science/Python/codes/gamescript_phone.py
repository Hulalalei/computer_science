### 开发者工具：手机IP:9096
### 1、悬赏每天需要手动做完
### 2、清空出部分背包
### 3、宠物卸下
from airscript.action import click
from airscript.action import slide
from airscript.action import touch
from ascript.android.screen import FindColors
import threading
import time

per_dur = 1
game_start = 15
every_dur = 10
flag = 0


def click_slp(x, y, snd, msg):
    click(x, y)
    print(msg)
    time.sleep(snd)


### flag = 1,悟空
### flag = 2,八戒
### flag = 3,沙僧
### flag = 4,无常
### flag = 5,唐僧
### flag = 6,白龙
### flag = 7,白骨
### flag = 8,龙女
### flag = 9,嫦娥
def initialize():
    # 滑倒屏幕最后
    i = 0
    while i < 3:
        slide(930, 895, 210, 759)
        time.sleep(1)
        i += 1

    # 点击 游戏文件夹
    click_slp(394, 1504, per_dur, "游戏文件夹-打开")
    # 点击 造梦4手游
    click_slp(539, 728, game_start, "造梦4-打开")
    # 点击 进入游戏
    click_slp(1293, 812, 2 * per_dur, "进入游戏")


### 选择角色
def init():
    if flag == 1:
        click_slp(1951, 142, per_dur, "选择悟空")
    if flag == 2:
        click_slp(1951, 320, per_dur, "选择八戒")
    if flag == 3:
        click_slp(1963, 504, per_dur, "选择沙僧")
    if flag == 4:
        click_slp(1951, 682, per_dur, "选择无常")
    if flag == 5:
        click_slp(2117, 219, per_dur, "选择唐僧")
    if flag == 6:
        click_slp(2111, 391, per_dur, "选择小白龙")
    if flag == 7:
        click_slp(2105, 569, per_dur, "选择白骨")
    if flag == 8:
        click_slp(2117, 789, per_dur, "选择龙女")
    if flag == 9:
        click_slp(2271, 338, per_dur, "嫦娥")
    # 点击开始游戏
    click_slp(2159, 931, every_dur, "开始游戏")
    click_slp(2052, 658, per_dur, "click")
    click_slp(2052, 658, per_dur, "click")
    click_slp(2052, 658, per_dur, "click")


next = True


def fight_other():
    global next
    while next:
        click(1755, 990)
        click(1821, 860)
        click(1910, 1002)
        click(1969, 812)
        click(2141, 765)
        click(2289, 967)
        click(2319, 777)
    print("timeout_fight")


def fight_ghost():
    global next
    while next:
        click(1743, 990)  # skill
        click(1910, 979)  # skill 1st
        click(1987, 824)
        click(2141, 765)  # skill 3rd
        click(2331, 759)  # fb
        click(2271, 949)  # fight
        click(1815, 836)  # change user
    print("timeout_fight")


def auto_fight():
    if flag == 4:
        fight_ghost()
    else:
        fight_other()


mv = True


def move_r():
    global mv
    while mv:
        click(427, 919)
    print("timeout_move")


def move_l():
    click(112, 919, 100)
    click(112, 919, 50)
    click(112, 919, 50)


def auto_thread(func, timeout):
    t = threading.Thread(target=func)
    t.start()
    time.sleep(timeout)
    return t


skill_time = 0
move_r_time = 6


def is_faery():
    f = FindColors.find("1204,670,#857F59|1174,753,#8B855C|1127,741,#7B7555|1138,664,#5B5A41|1245,664,#716D50")
    if f:
        click_slp(2313, 89, per_dur, "暂停")
        click_slp(1239, 718, per_dur, "退出关卡")
        click_slp(735, 842, per_dur, "返回主城")
        time.sleep(6)
        return True
    else:
        return False


def is_bat():
    f = FindColors.find("1373,146,#696853|1370,118,#070209|1373,168,#D6D0AB|1355,146,#989564|1390,146,#676347",
                        rect=[931, 47, 1826, 439])
    if f:
        click_slp(2313, 89, per_dur, "暂停")
        click_slp(1239, 718, per_dur, "退出关卡")
        click_slp(735, 842, per_dur, "返回主城")
        time.sleep(6)
        return True
    else:
        return False


def is_rain():
    f = FindColors.find("930,338,#2C2C17|933,260,#8FB375|878,328,#313018|999,336,#2C321F|946,407,#525937",
                        rect=[699, 100, 1773, 990])
    if f:
        click_slp(2313, 89, per_dur, "暂停")
        click_slp(1239, 718, per_dur, "退出关卡")
        click_slp(735, 842, per_dur, "返回主城")
        time.sleep(6)
        return True
    else:
        return False


def is_death():
    f = FindColors.find("960,795,#AF2E22|960,759,#BE3F34|960,812,#8F241A|925,777,#E9A79E|1002,795,#DD887D")
    if f:
        print("检测到角色死亡")
        click_slp(f.x, f.y, per_dur, "逃跑")


### 打一关
def bossround_complex(end_timeout):
    global next, mv
    i = 1
    while i <= 5:
        print("第", i, end=" ")
        print("波")

        mv = True
        t1 = auto_thread(move_r, move_r_time)
        mv = False
        t1.join()

        if is_death():
            time.sleep(end_timeout)
            return False

        move_l()

        next = True
        t2 = auto_thread(auto_fight, skill_time)
        while i <= 4:
            # 找"前进"
            f = FindColors.find("2105,404,#FF890E|2036,402,#FFBC12|2030,436,#FFB50C|2109,438,#FF8A0A|2144,411,#FF8609",
                                rect=[1915, 284, 2354, 551])
            if f:
                print("前进")
                next = False
                break
            else:
                time.sleep(1)
        if i == 5:
            while True:
                # 找"通关"
                f = FindColors.find(
                    "1221,551,#B55608|1168,534,#FFFFDF|1346,545,#FDFFDD|1210,308,#000000|1198,777,#000000")
                if f:
                    print("通关")
                    next = False
                    print("关卡结算倒计时")
                    time.sleep(end_timeout)
                    break

        t2.join()
        i += 1
    return True


### 刷关
def boss_infinite():
    global next, mv

    click_slp(2289, 985, per_dur, "战")
    click_slp(1198, 326, per_dur, "北俱芦洲")
    click_slp(670, 213, per_dur, "六耳")
    click_slp(1773, 747, 6 * per_dur, "挑战")

    while True:
        bossround_complex(10)
        click_slp(1755, 925, 3 * per_dur, "再次挑战")
        # 处理滑动验证
        # f = FindColors.find("896,707,#FAF3B8|898,662,#F3E488|891,742,#DA9C41|873,712,#FBF3B1|916,712,#FBF4B4",rect=[682,183,1702,884])
        # while f:
        #     slide(f.x, f.y, f.x + 10, f.y + 10,20)


### 扫荡
def sweep_high():
    click_slp(1565, 350, per_dur, "打开任务")
    click_slp(1815, 575, per_dur, "goto-3次打关")
    click_slp(486, 563, per_dur, "踏婆")
    click_slp(1761, 907, per_dur, "扫荡1次")
    click_slp(1435, 872, per_dur, "再次扫荡")
    click_slp(1435, 872, per_dur, "再次扫荡")
    click_slp(960, 907, per_dur, "扫荡-确认")
    click_slp(71, 17, per_dur, "返回地图")
    click_slp(71, 17, per_dur, "返回大地图")
    click_slp(71, 17, per_dur, "返回主界面")


def sweep_low():
    click_slp(2271, 985, per_dur, "战")
    click_slp(1198, 326, per_dur, "北俱芦洲")
    slide(747, 599, 1821, 540, 500)
    slide(747, 599, 1821, 540, 500)
    slide(747, 599, 1821, 540, 500)
    click_slp(729, 830, per_dur, "雨巫")
    click_slp(1761, 907, per_dur, "扫荡1次")
    click_slp(1435, 872, per_dur, "再次扫荡")
    click_slp(1435, 872, per_dur, "再次扫荡")
    click_slp(960, 907, per_dur, "扫荡-确认")
    click_slp(71, 17, per_dur, "返回地图")
    click_slp(71, 17, per_dur, "返回大地图")
    click_slp(71, 17, per_dur, "返回主界面")


def auto_sweep():
    if flag == 6:
        sweep_high()
    else:
        sweep_low()


### 强化装备
def enhance():
    click_slp(391, 480, per_dur, "点击铁匠")
    click_slp(1334, 569, per_dur, "选择装备")
    click_slp(1471, 551, per_dur, "选择装备")
    click_slp(1601, 545, per_dur, "选择装备")
    click_slp(854, 884, per_dur, "强化")
    click_slp(71, 17, per_dur, "返回任务")


### 升级坐骑
def mount():
    click_slp(860, 973, per_dur, "点击坐骑")
    click_slp(1565, 350, per_dur, "打开任务")
    click_slp(1791, 771, per_dur, "goto-坐骑")
    i = 0
    while i < 3:
        click_slp(1150, 563, per_dur, "切换坐骑")
        i += 1
    click_slp(705, 872, per_dur, "升级坐骑")
    click_slp(71, 17, per_dur, "返回主界面")


def auto_mount():
    if flag == 4 or flag == 6 or flag == 8:
        mount()
    else:
        print("no mount")


### 升级法宝
def mgc_weapon():
    click_slp(699, 979, per_dur, "点击法宝")
    click_slp(978, 866, per_dur, "选择法宝")
    click_slp(1186, 884, per_dur, "选择法宝")
    click_slp(1364, 878, per_dur, "选择法宝")
    click_slp(1542, 723, per_dur, "升级法宝")
    click_slp(71, 17, per_dur, "返回主界面")


### 商店购买
def shop():
    click_slp(1565, 350, per_dur, "打开任务")
    click_slp(1791, 771, per_dur, "goto-商店")
    click_slp(836, 427, per_dur, "点击商品1,1")
    click_slp(1364, 611, per_dur, "购买")
    click_slp(830, 777, per_dur, "点击商品1,2")
    click_slp(1364, 611, per_dur, "购买")
    click_slp(1162, 777, per_dur, "点击商品1,3")
    click_slp(1364, 611, per_dur, "购买")
    click_slp(71, 17, per_dur, "返回主界面")
    click_slp(1815, 575, per_dur, "领取-购买")
    click_slp(1198, 878, per_dur, "奖励确认")
    click_slp(71, 17, per_dur, "返回主界面")


### 锁妖塔
def tower():
    click_slp(2289, 985, per_dur, "战")
    click_slp(403, 272, per_dur, "点击锁妖塔")
    i = 0
    while i < 2:
        click_slp(664, 973, per_dur, "锁妖塔重置")
        click_slp(1382, 634, per_dur, "锁妖塔重置确认")
        click_slp(925, 985, per_dur, "扫荡")
        click_slp(1186, 907, per_dur, "锁妖塔跳过")
        click_slp(2052, 658, per_dur, "click")
        i += 1
    click_slp(71, 17, per_dur, "返回大地图")
    click_slp(71, 17, per_dur, "返回主界面")


def auto_tower():
    if flag == 4 or flag == 6 or flag == 8:
        tower()
    else:
        print("tower denied")


### 星宿
def constellation():
    click_slp(2289, 985, per_dur, "战")
    click_slp(1654, 160, per_dur, "星宿")
    i = 0
    while i < 3:
        click_slp(1690, 890, per_dur, "开始占星")
        click_slp(71, 17, per_dur, "返回主界面")
        click_slp(1654, 160, per_dur, "goto-星宿")
        click_slp(1737, 581, per_dur, "放弃")
        click_slp(1358, 629, per_dur, "放弃确认")
        i += 1

    click_slp(71, 17, per_dur, "返回主界面")
    click_slp(71, 17, per_dur, "返回主界面")


### 秘境挑战
def secret_area_high():
    click_slp(1579, 553, per_dur, "goto-秘境")
    click_slp(575, 373, per_dur, "秘境")
    click_slp(1791, 872, every_dur, "打鸟")
    click(415, 913, 3000)
    print("自杀")
    time.sleep(20)
    click_slp(996, 943, per_dur, "退出秘境")
    time.sleep(10)


def secret_area_mid():
    click_slp(1579, 553, per_dur, "goto-秘境")
    click_slp(575, 373, per_dur, "秘境")
    click_slp(1340, 860, every_dur, "打鸟")
    click(415, 913, 3000)
    print("自杀")
    time.sleep(20)
    click_slp(996, 943, per_dur, "退出秘境")
    time.sleep(10)


def secret_area_low():
    click_slp(1579, 553, per_dur, "goto-秘境")
    click_slp(575, 373, per_dur, "秘境")
    click_slp(806, 860, every_dur, "打八卦")
    time.sleep(20)
    click_slp(996, 943, per_dur, "退出秘境")
    time.sleep(10)


def auto_secret_area():
    if flag == 4 or flag == 6:  ### 秘境挑战
        secret_area_high()
    elif flag == 8:
        secret_area_mid()
    else:
        secret_area_low()


def coach():
    if flag != 4 and flag != 6:
        print("no coach")
        return

    j = 1
    while j <= 5:
        click_slp(1453, 575, per_dur, "goto-秘境")
        click_slp(557, 545, per_dur, "教官")
        click_slp(1506, 854, per_dur, "匹配")
        while True:
            f = FindColors.find("1234,526,#F38325|1209,421,#040000|1191,671,#520402|1078,542,#EB6F1D|1307,510,#F18F23",
                                rect=[925, 332, 1453, 729])
            if f:
                print("匹配到学员")
                time.sleep(10)
                break
        # 进入关卡了
        t = is_faery() or is_bat() or is_rain()
        if not t:
            if bossround_complex(10):
                # 关卡打完
                click_slp(1115, 492, 5 * per_dur, "点击宝箱")
                click_slp(1210, 872, per_dur, "确认宝箱")
                click_slp(1204, 1014, 8 * per_dur, "返回主城")
        j += 1


def pet():
    click_slp(806, 551, 4 * per_dur, "goto-backstreet")
    click_slp(213, 456, 2 * per_dur, "goto-pet")
    click_slp(1542, 326, per_dur, "get")
    click_slp(1488, 789, per_dur, "go!")
    click_slp(1334, 534, per_dur, "yes")
    time.sleep(5)
    click_slp(2295, 47, per_dur, "get rid1")
    click_slp(1233, 706, per_dur, "get rid2")
    click_slp(71, 17, per_dur, "返回主界面")
    time.sleep(5)
    click_slp(1554, 961, 5 * per_dur, "go back")


def auto_pet():
    if flag == 4 or flag == 6 or flag == 8:
        pet()
    else:
        print("no pet")


### 经文
def lection():
    i = 0
    while i <= 3:
        click_slp(438, 741, per_dur, "floor")
        time.sleep(3)
        i += 1

    click_slp(450, 468, per_dur, "goto-经文")
    click_slp(1067, 812, 3 * per_dur, "捐赠")
    i = 1
    while i <= 4:
        click_slp(1192, 718, 3 * per_dur, "再捐赠")
        i += 1
    click_slp(2052, 658, per_dur, "click")
    click_slp(71, 17, per_dur, "返回任务")


### 领取宝箱
def chest():
    click_slp(1565, 350, per_dur, "打开任务")
    while True:
        f = FindColors.find("1807,592,#AF5B10|1802,635,#EF7D28|1802,542,#FDE159|1751,589,#F29831|1856,593,#F4922F",
                            rect=[1488, 421, 2141, 842])
        if f:
            click_slp(f.x, f.y, per_dur, "领取-奖励")
            click_slp(1198, 878, per_dur, "奖励确认")
        else:
            break

    click_slp(1020, 872, per_dur, "宝箱1")
    click_slp(1186, 854, per_dur, "确认")
    click_slp(1174, 884, per_dur, "宝箱2")
    click_slp(1186, 854, per_dur, "确认")
    click_slp(1376, 872, per_dur, "宝箱3")
    click_slp(1186, 854, per_dur, "确认")
    click_slp(71, 17, per_dur, "返回主界面")


### 退到选角色界面
def exit():
    click_slp(118, 124, per_dur, "点击头像")
    click_slp(1435, 848, 5 * per_dur, "角色选择")


def daily_task():
    auto_sweep()  ### 扫荡，特判购买次数
    enhance()  ### 强化装备，金币、强化石
    auto_mount()  ### 升级坐骑，丹
    mgc_weapon()  ### 升级法宝，灵魂
    auto_tower()  ### 锁妖塔，背包
    constellation()  ### 星宿
    auto_secret_area()  ### 秘境
    coach()  ### 教官
    auto_pet()  ### 宠物
    lection()  ### 经文，金币、背包
    chest()  ### 领取宝箱
    exit()  ### 退到角色界面


arena_movetime = 4
### 点进去再开脚本
def arena():
    global next, mv
    while True:
        click_slp(1186, 979, per_dur, "开始战斗")

        while True:
            f = FindColors.find("1435,765,#84C256|1453,741,#95CB56|1435,807,#7BC949|1358,771,#A6D06D|1518,765,#84C252")
            if f:
                print("接受")
                click(f.x, f.y)
                break
        time.sleep(5)
        mv = True
        t1 = auto_thread(move_r, arena_movetime)
        mv = False
        t1.join()

        next = True
        t2 = auto_thread(auto_fight, skill_time)

        # 关卡结束判断
        while True:
            # 找"通关"
            f1 = FindColors.find("1198,925,#A72F0C|1138,925,#A72F0C|1281,925,#A92F0C|1192,884,#FCAC36|1186,967,#F37319")
            if f1:
                print("通关")
                next = False
                print("关卡结算倒计时")
                time.sleep(6)
                click_slp(1180, 931, 3 * per_dur, "返回主城")
                break
            else:
                time.sleep(3)
        t2.join()
        time.sleep(1)


def test():
    f = FindColors.find("1198,925,#A72F0C|1138,925,#A72F0C|1281,925,#A92F0C|1192,884,#FCAC36|1186,967,#F37319")
    if f:
        click_slp(f.x, f.y, per_dur, "返回主城")


### main():
def main():
    global flag
    # initialize()

    # i = 8
    # while i <= 9:
    #     flag = i
    #     init()
    #     daily_task()
    #     i += 1

    flag = 4
    # init()
    # daily_task()
    boss_infinite()
    # arena()
    # test()


main()