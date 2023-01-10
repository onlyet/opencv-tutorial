# opencv-tutorial-cpp（C++ OpenCV实战演示）
**包括编译，提取视频关键帧，图片特征提取和匹配，对图片/视频文件/网络摄像头进行目标检测**

## 开发环境
* win10
* vs2019（debug/x64）
* opencv-4.3.0（debug/x64）

*注意：本仓库只包含debug+x64平台的opencv库，VS选择平台的时候要选择debug+x64*

## 一 编译
### windows
* ~~[编译opencv4.3.0（启用surf算法） ](编译opencv4.3.0（使用surf算法）.md)~~

* [VS2017编译opencv4.7.0（启用surf算法和yolov3）](VS2017编译opencv4.7.0（使用surf算法和yolov3）.md)

### linux
* [centos7编译opencv](centos7编译opencv.md)


## 二 提取关键帧
* [C++调用opencv提取视频关键帧](./opencv-tutorial-cpp/ExtractKeyFrame/README.md)


## 三 特征点提取/匹配
* [C++调用opencv实现SURF/ORB算法对图片进行特征点提取/匹配](./opencv-tutorial-cpp/FeaturePointDemo/README.md)


## 四 目标检测
- [ ] darknet编译

* [使用darknet yolov3训练自己的数据集](./使用darknet-yolov3训练自己的数据集.md)
  
* [Qt调用opencv实现yolov3对视频进行目标检测](./opencv-tutorial-cpp/ObjectDetect/Qt调用opencv实现yolov3对视频进行目标检测.md)

## TODO
- [ ] 补充相关文档

## 最后
本项目重点是功能实现演示。作者水平有限，如有错误请指出。有什么问题可以提issue。  
如果觉得本项目对你有帮助，点个**star**吧！  
https://github.com/onlyet/opencv-tutorial  
B站的小伙伴给个**三连和关注**吧，你的四连是我更新的动力！
![Q群](QQ.png)