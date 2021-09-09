# GAMES101
Homeworks of GAMES101 courses. Library based on Eigen of 3.3.9 version.



Visual Studio 环境配置：

1、需要OpenCV软件库，以及Eigen开源模板库。

2、引入库的项目属性配置：

① **VC++目录**->**包含目录**，引入 **opencv\build\include** 所在路径；

② **VC++目录**->**库目录**，引入 **opencv\build\x64\vc15\lib** 所在路径；

③ **C/C++**->**附加包目录**，引入 **eigen-X.X.X\Eigen** 所在路径，这里需要特别注意的是，需要新建一个名为Eigen的文件夹（不是原目录下已有的同名文件夹），将 eigen-X.X.X 下所有文件夹剪切到新建的Eigen文件夹内，目录结构由eigen-X.X.X\变为eigen-X.X.X\Eigen）；

④ **链接器**->**输入**->**附加依赖项**，**opencv_world452.lib**    **opencv_world452d.lib** （需换行）；
