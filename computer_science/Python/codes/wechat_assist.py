from airscript.node import Selector
from airscript.action import key
from airscript.action import input
import time

while True:
    # 微信内检测到信息
    node = Selector(1).type("TextView").packageName("com.tencent.mm").parent(3).clickable(True).type("LinearLayout").click().find_all()
    if node:
        time.sleep(1)
        # 点击输入框，回复消息并发送
        Selector(1).type("EditText").packageName("com.tencent.mm").childCount(0).clickable(True).focusable(True).click().find_all()
        input("这憨批在敲代码，有事请留言（AI")
        time.sleep(1)
        Selector(1).type("Button").packageName("com.tencent.mm").text("发送").click().find_all()
        time.sleep(1)
        # 退出
        Selector(1).type("LinearLayout").packageName("com.tencent.mm").clickable(True).focusable(True).childCount(1).click().find_all()
        key.home()
    time.sleep(1)

    # 微信外检测到信息
    node = Selector(1).text("联系人发来").type("TextView").packageName("com.android.systemui").parent(4).click().find()
    if node:
        time.sleep(1)
        # 点击输入框，回复消息并发送
        Selector(1).type("EditText").packageName("com.tencent.mm").childCount(0).clickable(True).focusable(True).click().find_all()
        input("这憨批在敲代码，有事请留言（AI")
        time.sleep(1)
        Selector(1).type("Button").packageName("com.tencent.mm").text("发送").click().find_all()
        time.sleep(1)
        # 退出
        Selector(1).type("LinearLayout").packageName("com.tencent.mm").clickable(True).focusable(True).childCount(1).click().find_all()
        key.home()
