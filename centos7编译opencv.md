# 环境依赖
- centos7.5 
- opencv4.3.0 
- opencv_contrib-4.3.0 
- CUDA10.2 
- cudnn(和CUDA版本一致）
# 新建build.sh脚本，输入如下
```shell
cmake \
-D CMAKE_BUILD_TYPE=RELEASE \
-D BUILD_SHARED_LIBS=OFF \
-D CMAKE_INSTALL_PREFIX=/opt/opencv-4.3.0 \
-D OPENCV_EXTRA_MODULES_PATH=../opencv_contrib-4.3.0/modules \
-D CUDA_ARCH_BIN=6.1,7.5 \
-D CUDA_ARCH_PTX=7.5 \
-D OPENCV_ENABLE_NONFREE=ON \
-D ENABLE_CXX11=ON \
-D WITH_TBB=ON \
-D ENABLE_FAST_MATH=1 \
-D CUDA_FAST_MATH=1 \
-D WITH_CUBLAS=1 \
-D WITH_CUDA=ON \
-D WITH_CUDNN=ON \
-D OPENCV_DNN_CUDA=ON \
-D CUDA_NVCC_FLAGS="-D_FORCE_INLINES" \
-D WITH_GTK=OFF \
-D WITH_OPENJPEG=OFF \
-D WITH_1394=OFF \
-D BUILD_JAVA=OFF \
-D BUILD_TESTS=OFF \
-D BUILD_PERF_TESTS=OFF \
-D BUILD_opencv_apps=OFF \
-D BUILD_opencv_gapi=OFF \
-D BUILD_opencv_java_bindings_generator=OFF \
-D BUILD_opencv_js=OFF \
-D BUILD_opencv_python2=OFF \
-D BUILD_opencv_python3=OFF \
-D BUILD_opencv_python_bindings_generator=OFF \
-D BUILD_opencv_python_tests=OFF \
-D BUILD_opencv_ts=OFF \
-D BUILD_opencv_world=ON \
-D BUILD_opencv_xfeatures2d=ON \
-D BUILD_JASPER=ON \
-D BUILD_JPEG=ON \
-D BUILD_PNG=ON \
-D BUILD_PROTOBUF=ON \
-D BUILD_TIFF=ON \
-D BUILD_ZLIB=ON \
-D BUILD_WEBP=ON \
-D BUILD_OPENEXR=ON \
../opencv-4.3.0

```
## 选项解释

- CMAKE_BUILD_TYPE 生出release或debug
- BUILD_SHARED_LIBS 生出动态库或静态库
- OPENCV_EXTRA_MODULES_PATH contrib的module模块所在路径
- CUDA_ARCH_BIN 显卡对应的算力，具体可看CUDA维基百科
- OPENCV_ENABLE_NONFREE 为了使用surf等算法
- BUILD_opencv_world 只生成一个opencv_world.so/.a，否则每个模块生成一个.so/.a
- ../opencv-4.3.0 表示opencv源码cmakelist所再路径
- BUILD_开头的选项 表示使用opencv源码中的第三方库构建
