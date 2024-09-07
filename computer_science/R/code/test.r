# 关闭警告信息
options(warn = -1)

# 导入对应包
# library("ggplot2")
# library("dplyr")

# # 准备测试数据
# df <- data.frame(
#     sample = paste0("S", 1:4),
#     group1 = rep(c("A","B","C","D"), each=1),
#     group2 = rep(c("group1","group2"), times=2),
#     group3 = rep(c("CG","EG"), each=2),
#     value = sample(20:100, 4, replace = FALSE)
# )
# df$sample <- factor(df$sample, levels = df$sample)
# print(df$value)










# 测试01
# ggplot: df:数据集，aes: 坐标轴
# geom_bar(stat = "identity")
# geom_col(aes(fill = sample))：color边框颜色，fill填充颜色, = NA无填充，linewidth边框宽度
# geom_text(aes(y = value + 5, label = value))
# ggplot(df, aes(group1, value)) + 
    #geom_col(aes(fill = group2), position = position_dodge(width = 0.9)) #+ scale_fill_manual(values = c("#037ef3", "#f85a40", "#00c16e", "#7552cc")) #+ coord_flip()
    #geom_text(aes(y = value + 5, label = value)) + 
    #geom_bar(aes(fill = value), stat = "summary", fun = mean, position = "dodge") + 
    #geom_point(color = "black", size = 2.5)
# ggsave("./test.png")












#测试02
# 下载依赖包：BiocManager，并在此依赖下进行下载：BiocManager::install()
library("clusterProfiler")
library("org.Hs.eg.db")
library("dplyr")
library("DOSE")
library("ggplot2")
library("tidyr")
library("xlsx")

# 数据准备  行名不能重复 指定一下行名所处的列位置
results <- read.csv("./limma.result.csv", check.names = FALSE, row.names = 1)
# save(results, file = "./mydata.rda")  R中的二进制文件
# load(file = './mydata.rda')
# print(results)   # 控制台显示问题
# write.csv(results, "./results.csv")

# 筛选差异基因
# %>%：管道符，将左边的数据传递给右边
# filter：dplyr包下的筛选函数
# rownames()提取基因的行名
up <- results %>% filter(logFC > 1 & adj.P.Val < 0.5) %>% rownames()    # 上调基因symbol
down <- results %>% filter(logFC < -1 & adj.P.Val < 0.5) %>% rownames() # 下调基因symbol
diff <- c(up, down)




# KEGG富集
# diff_entrez <- bitr(
#     diff,
#     fromType = 'SYMBOL',
#     toType = 'ENTREZID',
#     OrgDb = 'org.Hs.eg.db'
# )
# head(diff_entrez)

# KEGG_enrich <- clusterProfiler::enrichKEGG(gene = diff_entrez$ENTREZID,
#                                                 organism = "hsa",
#                                                 pvalueCutoff = 0.05,
#                                                 qvalueCutoff = 0.05,
#                                                 pAdjustMethod = "BH",
#                                                 minGSSize = 10,
#                                                 maxGSSize = 500)

# KEGG_result <- KEGG_enrich@result
# write.csv(KEGG_result, "./KEGG.csv")


# library("enrichplot")
# # 气泡图
# dotplot(KEGG_enrich, x = "GeneRatio", showCategory = 10)
# ggsave("./KEGG_bubble.png")

# # 柱形图
# barplot(KEGG_enrich, x = "Count", color = "p.adjust", showCategory = 10)
# ggsave("./KEGG_bar.png")



# GO富集
diff_entrez <- bitr(
    diff,
    fromType = 'SYMBOL',
    toType = 'ENTREZID',
    OrgDb = 'org.Hs.eg.db'
)

go_enrich <- clusterProfiler::enrichGO(gene = diff_entrez$ENTREZID,
                                        ont = 'all',#可选'BP','CC','MF'or'all'
                                        keyType = "ENTREZID",
                                        OrgDb = org.Hs.eg.db,
                                        pAdjustMethod = "BH",#p值矫正方法
                                        pvalueCutoff = 0.05,
                                        qvalueCutoff = 0.05)

#将RNTREZ转换为Symbol
go_enrich <- DOSE::setReadable(go_enrich,
                                    OrgDb = org.Hs.eg.db,
                                    keyType = 'ENTREZID')

#去除冗余的GOterm
go_geo <- simplify(go_enrich, cutoff = 0.7, by = "p.adjust", select_fun = min)
#提取goG富集结果表格
go_result <- go_geo@result
# write.csv(GO_result, "./GO.csv")

# 气泡图
dotplot(go_geo,
            x = "GeneRatio",
            color = "p.adjust",
            showCategory = 10,
            split = 'ONTOLOGY',
            label_format = Inf) + #不换行
#分面
facet_grid(ONTOLOGY~.,
                space = 'free_y',#面板大小根据y轴自行调整
                scale = 'free_y'#子图坐标轴根据y轴自行调整
                )
ggsave("GO_bubble.png")

# 柱状图
barplot(go_geo,
            x = "Count",
            color = "p.adjust",
            showCategory = 10,
            split = 'ONTOLOGY',
            label_format = Inf)+
            facet_grid(ONTOLOGY~.,
            space = 'free_y',#面板大小根据y轴自行调整
            scale = 'free_y'#子图坐标轴根据y轴自行调整
            )
ggsave("GO_bar.png")