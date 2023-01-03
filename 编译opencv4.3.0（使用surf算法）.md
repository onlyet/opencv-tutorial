# 编译opencv4.3.0（使用surf算法）

# 为什么要重新编译opencv源码？
oepncv3之后要使用SURF,需要xfeatures2d模块，xfeatures2d模块被移到opencv_contrib了
所以要使用SURF，需要编译opencv源码和opencv_contrib源码
# 编译参考博客
[https://z90810839huanlan.zhihu.com/p/](https://zhuanlan.zhihu.com/p/90810839)  
[https://zhuanlan.zhihu.com/p/114526376](https://zhuanlan.zhihu.com/p/114526376)
# 可能出现问题
> 1>D:\Downloads\opencv-build\opencv_contrib-4.3.0\modules\xfeatures2d\test\test_features2d.cpp(51): fatal error C1083: 无法打开包括文件: “features2d/test/test_detectors_regression.impl.hpp”: No such file or directory
> 1>test_rotation_and_scale_invariance.cpp
> 1>D:\Downloads\opencv-build\opencv_contrib-4.3.0\modules\xfeatures2d\test\test_rotation_and_scale_invariance.cpp(7): fatal error C1083: 无法打开包括文件: “features2d/test/test_detectors_invariance.impl.hpp”: No such file or directory

原因是xfeature库还没成功编译
xfeature库依赖的有些预编译文件（boostdesc_bgm.i）在CMake的时候没有下载成功（网络问题），所以xfeature库没有编译成功
> fatal error: boostdesc_bgm.i: vgg_generated_80. vgg_generated_120.i没有那个文件或目录

解决参考：[https://blog.csdn.net/weixin_44023934/article/details/100586107](https://blog.csdn.net/weixin_44023934/article/details/100586107)
