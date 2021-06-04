#include <Eigen/Core>
#include <Eigen/Dense>
#include <cmath>
#include <iostream>

using namespace std;

/**
 * 给定一个点 (2, 1)，绕原点逆时针旋转45°，再平移 (1, 2)，计算变换后的坐标，用齐次坐标表示。
 *
 * 题解：因为默认右手系，所以逆时针旋转为正，绕原点旋转可以理解未绕Z轴旋转。
 * 弧度和角度的关系：1° = (PI / 180) arc，1 arc = (180 / PI)°
 * 可通过三角函数得到：PI = arccos(-1) = 4 * arctan(1)，
 * 因为arcsin(0)=sin(0)=sin(PI)，所以无法用正弦函数直接表示PI。
 */
void Homework()
{
	// PI/180 arc可表示为：acos(-1)/180 
	double degArc = acos(-1) / 180;

	// 旋转矩阵
	Eigen::Matrix3d rotM;
	rotM <<
		cos(45 * degArc), -sin(45 * degArc), 0,
		sin(45 * degArc), cos(45 * degArc), 0,
		0, 0, 1;
	// 平移矩阵
	Eigen::Matrix3d trsM;
	trsM <<
		1, 0, 1,
		0, 1, 2,
		0, 0, 1;

	// 矩阵与运算满足结合律，所以变换矩阵可为旋转矩阵和平移矩阵乘积 
	Eigen::Matrix3d conM = trsM * rotM; // 这里注意顺序，先旋转后平移，从右往左运算 
	cout << "Conversion Matrix:\n" << conM << endl;

	// 输入向量 
	Eigen::Vector3d inVec(2, 1, 1);

	cout << "Conversion coordinate:\n" << conM * inVec << endl;
}

int main() 
{
	Homework();

	return 0;
}