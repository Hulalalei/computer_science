# 关闭警告信息
options(warn = -1)

# 导入对应包
library("ggplot2")
library("dplyr")

# 准备测试数据
df <- data.frame(
  sample = paste0("S", 1:4),
  group1 = rep(c("A","B","C","D"), each=1),
  group2 = rep(c("group1","group2"), times=2),
  group3 = rep(c("CG","EG"), each=2),
  value = sample(20:100, 4, replace = FALSE)
)
df$sample <- factor(df$sample, levels = df$sample)
print(df$value)


# 饼图
# names = c("Google", "Runoob", "Taobao", "Weibo")
# cols = c("#ED1C24", "#22B14C", "#FFC90E", "#3f48CC")
# piepercent = paste(round(100 * df$value / sum(df$value)), "%") # 百分比计算
# png(file = "./temp.png", hight = 300, width = 300) # 存储图片的格式及大小
# pie(df$value, labels = piepercent, main = "网站分析", col = cols, family = 'GB1') # 画图
# legend("topright", names, cex = 0.8, fill = cols) # 添加标签

# 3D饼图
# library("plotrix")
# names = c("Google", "Runoob", "Taobao", "Weibo")
# cols = c("#ED1C24", "#22B14C", "#FFC90E", "#3f48CC")
# png(file = "./temp.png") # 存储图片的格式及大小
# pie3D(df$value, labels = names, explode = 0.1, main = "3D 图",family = "STHeitiTC-Light")

# 条形图---单条形图
# cvd19 = df$value
# png(file = "./temp.png")
# barplot(cvd19, main = '条形图', names.arg = c(1:4), col = c("#ED1C24", "#22B14C", "#FFC90E", "#3f48CC"))

# 条形图---双条形图
# cvd19 = matrix(
#   df$value,
#   2, 3
# )
# png(file = "temp.png")
# colnames(cvd19) = c("中国", "美国", "印度")
# rownames(cvd19) = c("6月", "7月")
# barplot(cvd19, main = "新冠疫情条形图", beside = FALSE, legend = TRUE, col = c("blue","green"), family = 'SyHei')

# 函数曲线图
# png(file = "temp.png")
# curve(sin(x), -2 * pi, 2 * pi)
# v <- c(7,12,28,3,41)
# png(file = "temp.png")
# plot(v,type = "o", col = "red", xlab = "Month", ylab = "Rain fall", main = "Rain fall chart")

# 散点图
# input <- mtcars[,c('wt','mpg')]
# png(file = "temp.png")
# plot(x = input$wt, y = input$mpg,
#   xlab = "Weight",
#   ylab = "Milage",
#   xlim = c(2.5,5),
#   ylim = c(15,30),
#   main = "Weight vs Milage"
# )
# png(file = "temp.png")
# pairs(~wt+mpg+disp+cyl, data = mtcars, main = "Scatterplot Matrix")


# p <- ggplot(df, aes(group1,value)) +
#   stat_summary(fun.data = 'mean_sd', geom = "errorbar", width = 0.15,size=1) +
#   geom_bar(aes(fill=group1),color="black",stat="summary",fun=mean,position="dodge",size=0.5)

# ggsave("plot.png", plot = p)