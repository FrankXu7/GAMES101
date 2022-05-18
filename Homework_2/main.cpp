// clang-format off
#include <iostream>
#include <opencv2/opencv.hpp>
#include "rasterizer.hpp"
#include "global.hpp"
#include "Triangle.hpp"

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

	model << 
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
	// TODO: Copy-paste your implementation from the previous assignment.
	Eigen::Matrix4f projection;

	// 记录 cot(θ/2) 便于计算
	float cot_half_fov = 1 / (tan(eye_fov * (MY_PI / 180) / 2));

	/**
	 * OpenCV采用左手系，且原点位于屏幕左上角；
	 * 作业框架也采用左手系，但原点位于屏幕左下角；
	 *
	 * 所以作业框架的图形在OpenCV环境中被渲染出来相当于绕X轴向屏幕外旋转了180°，
	 * 表现出的效果就是图形上下颠倒；
	 *
	 * 将Z轴取反即可保证在OpenCV中渲染出来的图形和作业框架中的保持一致
	 */
	float zSign = -1;

	// 图形上下翻转了，渲染的顺序自然也跟着翻转
	zNear *= -1;
	zFar *= -1;

	// 设置透视投影矩阵
	projection <<
		cot_half_fov / aspect_ratio, 0, 0, 0,
		0, cot_half_fov, 0, 0,
		0, 0, (zNear + zFar) / (zNear - zFar), -(2 * zNear * zFar) / (zNear - zFar),
		0, 0, zSign, 0;

	return projection;
}

int main(int argc, const char** argv)
{
	float angle = 0;
	bool command_line = false;
	std::string filename = "output.png";

	if (argc == 2)
	{
		command_line = true;
		filename = std::string(argv[1]);
	}

	rst::rasterizer r(700, 700);
	std::vector<Eigen::Vector3f> triangle;

	Eigen::Vector3f eye_pos = { 0,0,5 };


	std::vector<Eigen::Vector3f> pos
	{
			{2, 0, -2},
			{0, 2, -2},
			{-2, 0, -2},
			{3.5, -1, -5},
			{2.5, 1.5, -5},
			{-1, 0.5, -5}
	};

	std::vector<Eigen::Vector3i> ind
	{
			{0, 1, 2},
			{3, 4, 5}
	};

	std::vector<Eigen::Vector3f> cols
	{
			{217.0, 238.0, 185.0},
			{217.0, 238.0, 185.0},
			{217.0, 238.0, 185.0},
			{185.0, 217.0, 238.0},
			{185.0, 217.0, 238.0},
			{185.0, 217.0, 238.0}
	};

	auto pos_id = r.load_positions(pos);
	auto ind_id = r.load_indices(ind);
	auto col_id = r.load_colors(cols);

	int key = 0;
	int frame_count = 0;

	if (command_line)
	{
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

		r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

		cv::imwrite(filename, image);

		return 0;
	}

	while (key != 27)
	{
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

		r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);

		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("image", image);
		key = cv::waitKey(10);

		//std::cout << "frame count: " << frame_count++ << '\n';
	}

	return 0;
}
// clang-format on