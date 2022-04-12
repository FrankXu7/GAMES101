# GAMES101
Homeworks of GAMES101 courses. Library based on Eigen of 3.3.9 version.



Visual Studio 环境配置：

1. **Eigen**开源模板库。[[官网]](https://eigen.tuxfamily.org/index.php?title=Main_Page)
2. 安装（实际上是解压）**OpenCV**软件库，配置环境变量，路径为：**opencv\build\x64\vc15\bin**，需要**重启计算机后生效**。[[官网]](https://opencv.org/releases/)
3. 项目属性配置：

​	① **VC++目录**->**包含目录**，引入 **opencv\build\include** 所在路径；

​	② **VC++目录**->**库目录**，引入 **opencv\build\x64\vc15\lib** 所在路径；

​	③ **C/C++**->**附加包目录**，引入 **Eigen** 的根目录，这里需要注意的是，解压后根目录名称为 eigen-X.X.X，将其重命名为 Eigen 即可，因为GAMES101给的项目框架包含文件时是以Eigen命名的。

​	④ **链接器**->**输入**->**附加依赖项**，添加 **opencv_worldXXX.lib**  和 **opencv_worldXXXd.lib** 两个附加依赖项（位于目录：**opencv\build\x64\vc15\lib**），名称中**末尾带 'd'** 的表示针对**Debug版本**，**末尾不带 'd'** 的表示针对**Release版本**，其中的 **XXX** 表示的是**OpenCV的版本号**。比如安装的是 4.5.5 版本的OpenCV，且需要配置在Debug模式下运行，则附加依赖项添加的lib文件名称为：opencv_world455d.lib，需要特别注意Debug和Release版本的lib不可混用，**仅添加对应版本的lib文件即可**，否则可能导致OpenCV功能异常。