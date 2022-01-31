# GAMES101
Homeworks of GAMES101 courses. Library based on Eigen of 3.3.9 version.



Visual Studio 环境配置：

1. **Eigen**开源模板库。
2. 安装（实际上是解压）**OpenCV**软件库，配置环境变量，路径为：**opencv\build\x64\vc15\bin**，需要**重启计算机后生效**。
3. 项目属性配置：

​	① **VC++目录**->**包含目录**，引入 **opencv\build\include** 所在路径；

​	② **VC++目录**->**库目录**，引入 **opencv\build\x64\vc15\lib** 所在路径；

​	③ **C/C++**->**附加包目录**，引入 **Eigen** 的根目录，这里需要注意的是，解压后根目录名称为 eigen-X.X.X，将其重命名为 Eigen 即可，因为GAMES101给的项目框架包含文件时是以Eigen命名的。

​	④ **链接器**->**输入**->**附加依赖项**，添加 **opencv_worldXXX.lib**  和 **opencv_worldXXXd.lib** 两个附加依赖项，其中的 XXX 表示的是版本，比如安装的是 4.5.5 版本的OpenCV，那这两个附加依赖项的名称就是 opencv_world455.lib 和 opencv_world455d.lib，它们的路径是：**opencv\build\x64\vc15\lib**，按实际情况添加即可。
