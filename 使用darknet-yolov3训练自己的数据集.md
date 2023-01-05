# 使用darknet yolov3训练自己的数据集

 ## 准备
下载darknet并编译（GPU版），编译这里就不说了。反正视频检测要依赖opencv，GPU版本要依赖CUDA和cuDNN。

## 训练流程
*以训练人手和人脸这两个类为例子*

---

1. 将包含人手和人脸的图片放到build\darknet\x64\data\train_iamges和build\darknet\x64\data\val_images,train_iamges里是训练的图片，val_images里是用来验证的图片。

2. 新建 human.names文件，输入所有类名如下：（注意不要有制表符）
```
    human face
    human hand
```

1. 使用labelImg打标签，目的是生成每张图片对应的txt文件（保护框框的类序号，坐标）。labelImg保存一张图片时生成的是xml文件，我们需要将xml转为txt。
`python xml_to_txt.py`

1. 生成train.txt和val.txt文件，这两个文件里每一行都是图片的路径。
`python generate_train.py`

1. 新建 human.data文件，输入如下：
```
classes= 2
train = data/train.txt
valid = data/val.txt
names = data/human.names
backup = backup/
```

1. 新建human.cfg文件，里面的内容直接复制yolov3.cfg的，然后再修改：  

具体如何修改参考: https://github.com/AlexeyAB/darknet#how-to-train-to-detect-your-custom-objects  

主要修改如下：
```
# 已知我们的训练物体数量是2

# 训练物体数量*2000，即2*2000=4000
max_batches=4000 

# max_batches*0.8, max_batches*0.9
steps=3200,3600

# classes=训练物体数量
将610行，696行，783行改为classes=2

# filters=(classes + 5)x3
将603行，689行，776行改为filters=21
```

7.darknet训练命令和检测命令：

训练：
