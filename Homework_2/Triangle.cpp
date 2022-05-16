//
// Created by LEI XU on 4/11/19.
//

#include "Triangle.hpp"
#include <algorithm>
#include <array>


Triangle::Triangle() {
    v[0] << 0,0,0;
    v[1] << 0,0,0;
    v[2] << 0,0,0;

    color[0] << 0.0, 0.0, 0.0;
    color[1] << 0.0, 0.0, 0.0;
    color[2] << 0.0, 0.0, 0.0;

    tex_coords[0] << 0.0, 0.0;
    tex_coords[1] << 0.0, 0.0;
    tex_coords[2] << 0.0, 0.0;
}

void Triangle::setVertex(int ind, Vector3f ver){
    v[ind] = ver;
}
void Triangle::setNormal(int ind, Vector3f n){
    normal[ind] = n;
}
void Triangle::setColor(int ind, float r, float g, float b) {
    if((r<0.0) || (r>255.) ||
       (g<0.0) || (g>255.) ||
       (b<0.0) || (b>255.)) {
        fprintf(stderr, "ERROR! Invalid color values");
        fflush(stderr);
        exit(-1);
    }

    color[ind] = Vector3f((float)r/255.,(float)g/255.,(float)b/255.);
    return;
}
void Triangle::setTexCoord(int ind, float s, float t) {
    tex_coords[ind] = Vector2f(s,t);
}

std::array<Vector4f, 3> Triangle::toVector4() const
{
    std::array<Eigen::Vector4f, 3> res;
    std::transform(std::begin(v), std::end(v), res.begin(), [](auto& vec) { return Eigen::Vector4f(vec.x(), vec.y(), vec.z(), 1.f); });
    return res;
}

bool Triangle::insideTriangle(float x, float y) const
{
	// TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]

    // 计算三角形各边的向量
	Eigen::Vector3f TriangleVec1 = v[1] - v[0];
    Eigen::Vector3f TriangleVec2 = v[2] - v[1];
    Eigen::Vector3f TriangleVec3 = v[0] - v[2];

    // 计算给定点与三角形各顶点的向量
    Eigen::Vector3f PointVec(x, y, 0.f);
    Eigen::Vector3f PointVec1 = PointVec - v[0];
    Eigen::Vector3f PointVec2 = PointVec - v[1];
    Eigen::Vector3f PointVec3 = PointVec - v[2];

    // 保存叉乘结果
    float zValue1 = 0.f, zValue2 = 0.f, zValue3 = 0.f;

#if 1 // 使用Eigen内置叉乘接口
	zValue1 = PointVec1.cross(TriangleVec1)[2];
	zValue2 = PointVec2.cross(TriangleVec2)[2];
	zValue3 = PointVec3.cross(TriangleVec3)[2];
#else // 使用叉乘定义计算
    zValue1 = PointVec1[0] * TriangleVec1[1] - PointVec1[1] * TriangleVec1[0];
    zValue2 = PointVec2[0] * TriangleVec2[1] - PointVec2[1] * TriangleVec2[0];
    zValue3 = PointVec3[0] * TriangleVec3[1] - PointVec3[1] * TriangleVec3[0];
#endif

	return ((zValue1 >= 0 && zValue2 >= 0 && zValue3 >= 0) || (zValue1 <= 0 && zValue2 <= 0 && zValue3 <= 0));
}
