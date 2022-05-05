#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

	Eigen::Matrix4f translate;
	translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
		-eye_pos[2], 0, 0, 0, 1;

	view = translate * view;

	return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

	// TODO: Implement this function
	// Create the model matrix for rotating the triangle around the Z axis.
	// Then return it.

	// 计算给定角度的弧度值 
	float arcValue = rotation_angle * (MY_PI / 180);

	model <<
		cos(arcValue), -sin(arcValue), 0, 0,
		sin(arcValue), cos(arcValue), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
	float zNear, float zFar)
{
	// Students will implement this function

	Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

	// TODO: Implement this function
	// Create the projection matrix for the given parameters.
	// Then return it.

#if 0

	/** 已知视场角、宽高比以及远/近距离，直接按公式写出透视投影矩阵  */

	float cot_half_fov = 1 / (tan(eye_fov * (MY_PI / 180) / 2)); // 即 cot(θ/2) 的值 
	projection <<
		cot_half_fov / aspect_ratio, 0, 0, 0,
		0, cot_half_fov, 0, 0,
		0, 0, (zNear + zFar) / (zNear - zFar), -(2 * zNear * zFar / (zNear - zFar)),
		0, 0, -1, 0; // 默认情况下是从-Z看向+Z，但给出的eye_pos是(0,0,5)，看向的方向和公式默认方向相反，故齐次坐标Z轴取反

#else

	/** 透视投影先“挤压”到长方体，然后再进行正交投影（先平移，再缩放） */

	// 依据 tan(FovY) = t/n，已知视场角FovY和近平面n，可求高度t
	float t = tan(eye_fov / 2.f) * zNear, b = -t;
	// 依据 ascept ratio = r/t，已知宽高比ascept ratio和高度t，可求宽度r
	float r = t * aspect_ratio, l = -r;

	// 正交投影：空间中心平移至原点
	Eigen::Matrix4f OrthoTranstion = Eigen::Matrix4f::Identity();
	OrthoTranstion <<
		1, 0, 0, -((r + l) / 2),
		0, 1, 0, -((t + b) / 2),
		0, 0, 1, -((zNear + zFar) / 2),
		0, 0, 0, 1;

	// 正交投影：坐标轴缩放至 [-1, 1]³ 的标准立方体中
	Eigen::Matrix4f OrthoScale = Eigen::Matrix4f::Identity();
	OrthoScale <<
		2 / (r - l), 0, 0, 0,
		0, 2 / (t - b), 0, 0,
		0, 0, 2 / (zNear - zFar), 0,
		0, 0, 0, 1;

	// 透视投影：挤压到长方体（挤压后再进行正交投影）
	Eigen::Matrix4f PerspOrtho = Eigen::Matrix4f::Identity();
	PerspOrtho <<
		zNear, 0, 0, 0,
		0, zNear, 0, 0,
		0, 0, zNear + zFar, -(zNear * zFar),
		0, 0, -1, 0; // 默认情况下是从-Z看向+Z，但给出的eye_pos是(0,0,5)，看向的方向和公式默认方向相反，故齐次坐标Z轴取反

	projection = OrthoScale * OrthoTranstion * PerspOrtho;

#endif

	return projection;
}

int main(int argc, const char** argv)
{
	float angle = 0;
	bool command_line = false;
	std::string filename = "output.png";

	if (argc >= 3) {
		command_line = true;
		angle = std::stof(argv[2]); // -r by default
		if (argc == 4) {
			filename = std::string(argv[3]);
		}
	}

	rst::rasterizer r(700, 700);

	Eigen::Vector3f eye_pos = { 0, 0, 5 };

	std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };

	std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };

	auto pos_id = r.load_positions(pos);
	auto ind_id = r.load_indices(ind);

	int key = 0;
	int frame_count = 0;

	if (command_line) {
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

		r.draw(pos_id, ind_id, rst::Primitive::Triangle);
		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);

		cv::imwrite(filename, image);

		return 0;
	}

	while (key != 27) {
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

		r.draw(pos_id, ind_id, rst::Primitive::Triangle);

		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::imshow("image", image);
		key = cv::waitKey(10);

		//std::cout << "frame count: " << frame_count++ << '\n';

		if (key == 'a' || key == 'A') angle += 5;
		else if (key == 'd' || key == 'D') angle -= 5;

		std::cout << "CurAngle: " << angle << std::endl;
	}

	return 0;
}
