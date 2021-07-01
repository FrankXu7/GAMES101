#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

	Eigen::Matrix4f translate;
	translate <<
		1, 0, 0, -eye_pos[0],
		0, 1, 0, -eye_pos[1],
		0, 0, 1, -eye_pos[2],
		0, 0, 0, 1;

	view = translate * view;

	return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

	// TODO: Implement this function 
	// Create the model matrix for rotating the triangle around the Z axis. 
	// Then return it. 

	// 转换为弧度制  
	float angleArc = static_cast<float>(rotation_angle * (MY_PI / 180));

	// 减少一点计算量  
	float calcSin = sin(angleArc);
	float calcCos = cos(angleArc);

	model <<
		calcCos, -calcSin, 0, 0,
		calcSin, calcCos, 0, 0,
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

	// 矩阵中的 cot(fov/2) 
	float fovCot = static_cast<float>(1 / tan((eye_fov / 2) * (MY_PI / 180)));

	projection <<
		(1 / aspect_ratio) * fovCot, 0, 0, 0,
		0, fovCot, 0, 0,
		0, 0, ((zNear + zFar) / (zNear - zFar)), -((2 * zNear * zFar) / (zNear - zFar)),
		0, 0, 1, 0;

	return projection;
}

int main(int argc, const char** argv)
{
	float angle = 45;
	//bool command_line = false;
	//std::string filename = "output.png";

	// 在 Visual Studio 上配置环境不需要用这个
	//if (argc >= 3) {
	//	command_line = true;
	//	angle = std::stof(argv[2]); // -r by default 
	//	if (argc == 4) {
	//		filename = std::string(argv[3]);
	//	}
	//}

	rst::rasterizer r(700, 700);

	Eigen::Vector3f eye_pos = { 0, 0, 5 };

	std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };

	std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };

	auto pos_id = r.load_positions(pos);
	auto ind_id = r.load_indices(ind);

	int key = 0;
	int frame_count = 0;

	// 在 Visual Studio 上配置环境不需要用这个
	//if (command_line) {
	//	r.clear(rst::Buffers::Color | rst::Buffers::Depth);

	//	r.set_model(get_model_matrix(angle));
	//	r.set_view(get_view_matrix(eye_pos));
	//	r.set_projection(get_projection_matrix(45.f, 1, 0.1f, 50.f));

	//	r.draw(pos_id, ind_id, rst::Primitive::Triangle);
	//	cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
	//	image.convertTo(image, CV_8UC3, 1.0f);

	//	cv::imwrite(filename, image);

	//	return 0;
	//}

	while (key != 27) {
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45.f, 1.f, 0.1f, 50.f));

		r.draw(pos_id, ind_id, rst::Primitive::Triangle);

		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::imshow("image", image);
		key = cv::waitKey(100);

		angle += 0.5;
		std::cout << "frame count: " << frame_count++ << "\t Angle: " << angle << '\n';

		if (key == 'a') {
			angle += 10;
			cout << endl << angle << endl;
		}
		else if (key == 'd') {
			angle -= 10;
			cout << endl << angle << endl;
		}
	}

	return 0;
}
