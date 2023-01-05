# C++调用opencv提取视频关键帧

# 依赖opencv模块
* opencv_core430d.lib
* opencv_highgui430d.lib
* opencv_imgproc430d.lib
* opencv_imgcodecs430d.lib
* opencv_videoio430d.lib
 
- opencv_core430d.dll
- opencv_highgui430d.dll
- opencv_imgproc430d.dll
- opencv_imgcodecs430d.dll
- opencv_videoio430d.dll
- opencv_videoio_ffmpeg430_64.dll（没有该dll不能打开视频）

# 原理
使用灰度帧差法提取关键帧
> 我们将视频第一帧设定为关键帧，后面的每一个帧与前一个关键帧进行像素帧差(灰度图)，并设定阈值判断发生变化的像素点比例，通过此比例判断帧是否有发生突变，有发生突变的帧设定为关键帧并保存在特定文件夹中。以此循环将所有帧遍历完成即可。

# 运行项目
VS将ExtractKeyFrame设为启动项目，f5调试  
程序会一直更新显示关键帧

# 演示
<img decoding="async" src="./demo.png" width="80%">

# 参考
https://blog.csdn.net/weixin_39704651/article/details/90261357