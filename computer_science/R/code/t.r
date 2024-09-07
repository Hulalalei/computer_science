
# 猜数字
print("请猜一个数字: ")
num = readline()

# 通过更改target的值来设置答案
target = 76

while (num != target) {
	if (num > target) {
		print("猜大了")
	}
	else {
		print("猜小了")
	}
	num = readline()
}
print("恭喜你！猜对了")
