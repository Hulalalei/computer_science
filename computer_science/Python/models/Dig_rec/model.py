# 导入必要的库
import numpy as np
import matplotlib.pyplot as plt
from tensorflow.keras.datasets import mnist
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Flatten, Dropout
from tensorflow.keras.utils import to_categorical
from tensorflow.keras.optimizers import Adam

# 1. 数据加载与预处理
def load_and_preprocess_data():
    """加载MNIST数据集并进行预处理"""
    # 加载MNIST数据集（自动下载，若已下载则读取本地文件）
    (x_train, y_train), (x_test, y_test) = mnist.load_data()
    
    # 数据形状查看
    print(f"训练集形状: {x_train.shape}, 训练标签形状: {y_train.shape}")
    print(f"测试集形状: {x_test.shape}, 测试标签形状: {y_test.shape}")
    
    # 数据归一化：将像素值从0-255缩放到0-1
    x_train = x_train.astype('float32') / 255.0
    x_test = x_test.astype('float32') / 255.0
    
    # 标签独热编码（将0-9的数字转换为10维向量）
    y_train = to_categorical(y_train, 10)
    y_test = to_categorical(y_test, 10)
    
    return (x_train, y_train), (x_test, y_test)

# 2. 构建深度神经网络模型
def build_model(input_shape=(28, 28), num_classes=10):
    """构建一个简单的全连接神经网络模型"""
    model = Sequential([
        # 将28x28的二维图像展平为784维向量
        Flatten(input_shape=input_shape),
        # 第一个全连接层：512个神经元，ReLU激活函数
        Dense(512, activation='relu'),
        # Dropout层：随机失活20%的神经元，防止过拟合
        Dropout(0.2),
        # 第二个全连接层：256个神经元，ReLU激活函数
        Dense(256, activation='relu'),
        # 输出层：10个神经元（对应10个类别），Softmax激活函数
        Dense(num_classes, activation='softmax')
    ])
    
    # 打印模型结构
    model.summary()
    return model

# 3. 训练模型
def train_model(model, x_train, y_train, x_test, y_test, epochs=10, batch_size=128):
    """训练模型并返回训练历史"""
    # 编译模型：指定优化器、损失函数和评估指标
    model.compile(
        optimizer=Adam(learning_rate=0.001),  # Adam优化器
        loss='categorical_crossentropy',      # 多分类交叉熵损失
        metrics=['accuracy']                  # 评估指标：准确率
    )
    
    # 训练模型
    history = model.fit(
        x_train, y_train,
        epochs=epochs,                        # 训练轮次
        batch_size=batch_size,                # 批次大小
        validation_split=0.1,                 # 从训练集中划分10%作为验证集
        verbose=1                             # 显示训练过程
    )
    
    return history

# 4. 评估模型
def evaluate_model(model, x_test, y_test):
    """在测试集上评估模型性能"""
    print("\n测试集评估结果：")
    loss, accuracy = model.evaluate(x_test, y_test, verbose=0)
    print(f"测试集损失: {loss:.4f}")
    print(f"测试集准确率: {accuracy:.4f}")
    return loss, accuracy

# 5. 模型预测示例
def predict_samples(model, x_test, y_test, num_samples=5):
    """随机选择样本进行预测并可视化结果"""
    # 随机选择样本索引
    indices = np.random.choice(len(x_test), num_samples, replace=False)
    
    # 预测
    predictions = model.predict(x_test[indices])
    # 转换为预测类别（取概率最大的类别）
    pred_classes = np.argmax(predictions, axis=1)
    # 真实类别
    true_classes = np.argmax(y_test[indices], axis=1)
    
    # 可视化结果
    plt.figure(figsize=(10, 4))
    for i, idx in enumerate(indices):
        plt.subplot(1, num_samples, i+1)
        plt.imshow(x_test[idx], cmap='gray')
        plt.title(f"预测: {pred_classes[i]}\n真实: {true_classes[i]}")
        plt.axis('off')
    plt.tight_layout()
    plt.show()

# 主函数：串联所有流程
def main():
    # 加载并预处理数据
    (x_train, y_train), (x_test, y_test) = load_and_preprocess_data()
    
    # 构建模型
    model = build_model()
    
    # 训练模型
    history = train_model(model, x_train, y_train, x_test, y_test, epochs=10, batch_size=128)
    
    # 评估模型
    evaluate_model(model, x_test, y_test)
    
    # 预测示例
    predict_samples(model, x_test, y_test)
    
    # 绘制训练过程中的准确率和损失曲线
    plt.figure(figsize=(12, 4))
    # 准确率曲线
    plt.subplot(1, 2, 1)
    plt.plot(history.history['accuracy'], label='训练准确率')
    plt.plot(history.history['val_accuracy'], label='验证准确率')
    plt.title('模型准确率')
    plt.xlabel('Epoch')
    plt.ylabel('准确率')
    plt.legend()
    
    # 损失曲线
    plt.subplot(1, 2, 2)
    plt.plot(history.history['loss'], label='训练损失')
    plt.plot(history.history['val_loss'], label='验证损失')
    plt.title('模型损失')
    plt.xlabel('Epoch')
    plt.ylabel('损失')
    plt.legend()
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()
