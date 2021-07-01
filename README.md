# GAMES101
Homeworks of GAMES101 courses. Library based on Eigen of 3.3.9 version.



Visual Studio 环境配置：

1、需要OpenCV软件库，以及Eigen开源模板库。

2、引入库的项目属性配置：

① VC++目录中**包含目录**，引入 **opencv\build\include** 所在路径；

② VC++目录中**库目录**，引入 **opencv\build\x64\vc15\lib** 所在路径；

③ C/C++**附加包目录**，引入 **eigen3\Eigen** 所在路径；

④ 链接器输入**附加依赖项**，**opencv_world452.lib**    **opencv_world452d.lib** ；