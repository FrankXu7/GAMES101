#include <iostream>
#include <opencv2/opencv.hpp>

#include "global.hpp"
#include "rasterizer.hpp"
#include "Triangle.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "OBJ_Loader.h"

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

Eigen::Matrix4f get_model_matrix(float angle)
{
	Eigen::Matrix4f rotation;
	angle = angle * MY_PI / 180.f;
	rotation <<
		cos(angle), 0, sin(angle), 0,
		0, 1, 0, 0,
		-sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1;

	Eigen::Matrix4f scale;
	scale <<
		2.5, 0, 0, 0,
		0, 2.5, 0, 0,
		0, 0, 2.5, 0,
		0, 0, 0, 1;

	Eigen::Matrix4f translate;
	translate <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	return translate * rotation * scale;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
	// TODO: Use the same projection matrix from the previous assignments

	/**
	 * OpenVC左手法则，以左上角为原点；
	 * 作业框架也是左手法则，但以左下角为原点；
	 *
	 * 作业框架的图形被OpenCV渲染后会翻转180°，所以，将齐次矩阵中的Z值取反，
	 * 相当于摄像机旋转了180°，这样OpenCV渲染的图形就和作业框架保持一致了
	 */
	float zSign = -1;

	/**
	 * 注意摄像机翻转之后，其向上方向时保持不变的，
	 * 所以远近距离也需要取反，保证和摄像机不在同一侧
	 */
	zNear *= -1;
	zFar *= -1;

	// 记录 cot(θ/2) 的值便于计算
	float cot_half_fov = 1 / tan((eye_fov / 2) * (MY_PI / 180));

	Eigen::Matrix4f ProjectionMx;
	ProjectionMx <<
		0, cot_half_fov / aspect_ratio, 0, 0,
		0, cot_half_fov, 0, 0,
		0, 0, (zNear + zFar) / (zNear - zFar), -(2 * zNear * zFar) / (zNear - zFar),
		0, 0, zSign, 0;

	return ProjectionMx;
}

Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload)
{
	return payload.position;
}

Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload)
{
	Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
	Eigen::Vector3f result;
	result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
	return result;
}

static Eigen::Vector3f reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis)
{
	auto costheta = vec.dot(axis);
	return (2 * costheta * axis - vec).normalized();
}

struct light
{
	Eigen::Vector3f position;
	Eigen::Vector3f intensity;
};

Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
{
	Eigen::Vector3f return_color = { 0, 0, 0 };
	if (payload.texture)
	{
		// TODO: Get the texture value at the texture coordinates of the current fragment

	}
	Eigen::Vector3f texture_color;
	texture_color << return_color.x(), return_color.y(), return_color.z();

	Eigen::Vector3f ka = Eigen::Vector3f(0.005f, 0.005f, 0.005f);
	Eigen::Vector3f kd = texture_color / 255.f;
	Eigen::Vector3f ks = Eigen::Vector3f(0.7937f, 0.7937f, 0.7937f);

	auto l1 = light{ {20, 20, 20}, {500, 500, 500} };
	auto l2 = light{ {-20, 20, 0}, {500, 500, 500} };

	std::vector<light> lights = { l1, l2 };
	Eigen::Vector3f amb_light_intensity{ 10, 10, 10 };
	Eigen::Vector3f eye_pos{ 0, 0, 10 };

	float p = 150;

	Eigen::Vector3f color = texture_color;
	Eigen::Vector3f point = payload.view_pos;
	Eigen::Vector3f normal = payload.normal;

	Eigen::Vector3f result_color = { 0, 0, 0 };

	for (auto& light : lights)
	{
		// TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
		// components are. Then, accumulate that result on the *result_color* object.

	}

	return result_color * 255.f;
}

Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
{
	Eigen::Vector3f ka = Eigen::Vector3f(0.005f, 0.005f, 0.005f);
	Eigen::Vector3f kd = payload.color;
	Eigen::Vector3f ks = Eigen::Vector3f(0.7937f, 0.7937f, 0.7937f);

	auto l1 = light{ {20, 20, 20}, {500, 500, 500} };
	auto l2 = light{ {-20, 20, 0}, {500, 500, 500} };

	std::vector<light> lights = { l1, l2 };
	Eigen::Vector3f amb_light_intensity{ 10, 10, 10 };
	Eigen::Vector3f eye_pos{ 0, 0, 10 };

	float p = 150;

	Eigen::Vector3f color = payload.color;
	Eigen::Vector3f point = payload.view_pos;
	Eigen::Vector3f normal = payload.normal;

	Eigen::Vector3f result_color = { 0, 0, 0 };
	for (auto& light : lights)
	{
		// TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
		// components are. Then, accumulate that result on the *result_color* object.

	}

	return result_color * 255.f;
}



Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload)
{

	Eigen::Vector3f ka = Eigen::Vector3f(0.005f, 0.005f, 0.005f);
	Eigen::Vector3f kd = payload.color;
	Eigen::Vector3f ks = Eigen::Vector3f(0.7937f, 0.7937f, 0.7937f);

	auto l1 = light{ {20, 20, 20}, {500, 500, 500} };
	auto l2 = light{ {-20, 20, 0}, {500, 500, 500} };

	std::vector<light> lights = { l1, l2 };
	Eigen::Vector3f amb_light_intensity{ 10, 10, 10 };
	Eigen::Vector3f eye_pos{ 0, 0, 10 };

	float p = 150;

	Eigen::Vector3f color = payload.color;
	Eigen::Vector3f point = payload.view_pos;
	Eigen::Vector3f normal = payload.normal;

	float kh = 0.2f, kn = 0.1f;

	// TODO: Implement displacement mapping here
	// Let n = normal = (x, y, z)
	// Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
	// Vector b = n cross product t
	// Matrix TBN = [t b n]
	// dU = kh * kn * (h(u+1/w,v)-h(u,v))
	// dV = kh * kn * (h(u,v+1/h)-h(u,v))
	// Vector ln = (-dU, -dV, 1)
	// Position p = p + kn * n * h(u,v)
	// Normal n = normalize(TBN * ln)


	Eigen::Vector3f result_color = { 0, 0, 0 };

	for (auto& light : lights)
	{
		// TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
		// components are. Then, accumulate that result on the *result_color* object.


	}

	return result_color * 255.f;
}


Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload)
{

	Eigen::Vector3f ka = Eigen::Vector3f(0.005f, 0.005f, 0.005f);
	Eigen::Vector3f kd = payload.color;
	Eigen::Vector3f ks = Eigen::Vector3f(0.7937f, 0.7937f, 0.7937f);

	auto l1 = light{ {20, 20, 20}, {500, 500, 500} };
	auto l2 = light{ {-20, 20, 0}, {500, 500, 500} };

	std::vector<light> lights = { l1, l2 };
	Eigen::Vector3f amb_light_intensity{ 10, 10, 10 };
	Eigen::Vector3f eye_pos{ 0, 0, 10 };

	float p = 150;

	Eigen::Vector3f color = payload.color;
	Eigen::Vector3f point = payload.view_pos;
	Eigen::Vector3f normal = payload.normal;


	float kh = 0.2f, kn = 0.1f;

	// TODO: Implement bump mapping here
	// Let n = normal = (x, y, z)
	// Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
	// Vector b = n cross product t
	// Matrix TBN = [t b n]
	// dU = kh * kn * (h(u+1/w,v)-h(u,v))
	// dV = kh * kn * (h(u,v+1/h)-h(u,v))
	// Vector ln = (-dU, -dV, 1)
	// Normal n = normalize(TBN * ln)


	Eigen::Vector3f result_color = { 0, 0, 0 };
	result_color = normal;

	return result_color * 255.f;
}

int main(int argc, const char** argv)
{
	std::vector<Triangle*> TriangleList;

	float angle = 140.f;
	bool command_line = true;

	std::string filename = "output.png";
	objl::Loader Loader;
	std::string obj_path = "GAMES101/Homework_3/Code/models/spot/";

	// Load .obj File
	bool loadout = Loader.LoadFile("GAMES101/Homework_3/Code/models/spot/spot_triangulated_good.obj");
	for (auto mesh : Loader.LoadedMeshes)
	{
		for (int i = 0; i < mesh.Vertices.size(); i += 3)
		{
			Triangle* t = new Triangle();
			for (int j = 0; j < 3; j++)
			{
				t->setVertex(j, Vector4f(mesh.Vertices[i + j].Position.X, mesh.Vertices[i + j].Position.Y, mesh.Vertices[i + j].Position.Z, 1.f));
				t->setNormal(j, Vector3f(mesh.Vertices[i + j].Normal.X, mesh.Vertices[i + j].Normal.Y, mesh.Vertices[i + j].Normal.Z));
				t->setTexCoord(j, Vector2f(mesh.Vertices[i + j].TextureCoordinate.X, mesh.Vertices[i + j].TextureCoordinate.Y));
			}
			TriangleList.push_back(t);
		}
	}

	rst::rasterizer r(700, 700);

	auto texture_path = "hmap.jpg";
	r.set_texture(Texture(obj_path + texture_path));

	std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = phong_fragment_shader;

	//if (argc >= 2)
	//{
	//	command_line = true;
	//	filename = std::string(argv[1]);
	//	if (argc == 3 && std::string(argv[2]) == "texture")
	//	{
	//		std::cout << "Rasterizing using the texture shader\n";
	//		active_shader = texture_fragment_shader;
	//		texture_path = "spot_texture.png";
	//		r.set_texture(Texture(obj_path + texture_path));
	//	}
	//	else if (argc == 3 && std::string(argv[2]) == "normal")
	//	{
	//		std::cout << "Rasterizing using the normal shader\n";
	//		active_shader = normal_fragment_shader;
	//	}  
	//	else if (argc == 3 && std::string(argv[2]) == "phong")
	//	{
	//		std::cout << "Rasterizing using the phong shader\n";
	//		active_shader = phong_fragment_shader;
	//	}
	//	else if (argc == 3 && std::string(argv[2]) == "bump")
	//	{
	//		std::cout << "Rasterizing using the bump shader\n";
	//		active_shader = bump_fragment_shader;
	//	}
	//	else if (argc == 3 && std::string(argv[2]) == "displacement")
	//	{
	//		std::cout << "Rasterizing using the bump shader\n";
	//		active_shader = displacement_fragment_shader;
	//	}
	//}

	/**
	 * 间接实现前面main函数参数数里argc>2的分支内容
	 */
	static std::map<std::string, int> ArgsMap = {
		{"texture", 1}, {"normal", 2}, {"phong", 3}, {"bump", 4}, {"displacement", 5},
	};

	switch (ArgsMap["texture"])
	{
		case 1: // texture
			std::cout << "Rasterizing using the texture shader\n";
			active_shader = texture_fragment_shader;
			texture_path = "spot_texture.png";
			r.set_texture(Texture(obj_path + texture_path));
			break;
		case 2: // normal
			std::cout << "Rasterizing using the normal shader\n";
			active_shader = normal_fragment_shader;
			break;
		case 3: // phong
			std::cout << "Rasterizing using the phong shader\n";
			active_shader = phong_fragment_shader;
			break;
		case 4: // bump
			std::cout << "Rasterizing using the bump shader\n";
			active_shader = bump_fragment_shader;
			break;
		case 5: // displacement
			std::cout << "Rasterizing using the bump shader\n";
			active_shader = displacement_fragment_shader;
			break;

		default: std::cout << "Unknow Rasterizing type!\n"; break;
	}

	Eigen::Vector3f eye_pos = { 0,0,10 };

	r.set_vertex_shader(vertex_shader);
	r.set_fragment_shader(active_shader);

	int key = 0;
	int frame_count = 0;

	if (command_line)
	{
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);
		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45.f, 1.f, 0.1f, 50.f));

		r.draw(TriangleList);
		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

		cv::imwrite(filename, image);

		return 0;
	}

	while (key != 27)
	{
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45.f, 1.f, 0.1f, 50.f));

		//r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
		r.draw(TriangleList);
		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

		cv::imshow("image", image);
		cv::imwrite(filename, image);
		key = cv::waitKey(10);

		if (key == 'a')
		{
			angle -= 0.1f;
		}
		else if (key == 'd')
		{
			angle += 0.1f;
		}

	}
	return 0;
}
