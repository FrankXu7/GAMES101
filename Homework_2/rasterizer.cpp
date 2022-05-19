// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f>& positions)
{
	auto id = get_next_id();
	pos_buf.emplace(id, positions);

	return { id };
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i>& indices)
{
	auto id = get_next_id();
	ind_buf.emplace(id, indices);

	return { id };
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f>& cols)
{
	auto id = get_next_id();
	col_buf.emplace(id, cols);

	return { id };
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
	return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
	auto& buf = pos_buf[pos_buffer.pos_id];
	auto& ind = ind_buf[ind_buffer.ind_id];
	auto& col = col_buf[col_buffer.col_id];

	float f1 = (50 - 0.1f) / 2.f;
	float f2 = (50 + 0.1f) / 2.f;

	Eigen::Matrix4f mvp = projection * view * model;
	for (auto& i : ind)
	{
		Triangle t;
		Eigen::Vector4f v[] = {
				mvp * to_vec4(buf[i[0]], 1.0f),
				mvp * to_vec4(buf[i[1]], 1.0f),
				mvp * to_vec4(buf[i[2]], 1.0f)
		};
		//Homogeneous division
		for (auto& vec : v) {
			vec /= vec.w();
		}
		//Viewport transformation
		for (auto& vert : v)
		{
			vert.x() = 0.5f * width * (vert.x() + 1.f);
			vert.y() = 0.5f * height * (vert.y() + 1.f);
			vert.z() = vert.z() * f1 + f2;
		}

		for (int i = 0; i < 3; ++i)
		{
			t.setVertex(i, v[i].head<3>());
			t.setVertex(i, v[i].head<3>());
			t.setVertex(i, v[i].head<3>());
		}

		auto col_x = col[i[0]];
		auto col_y = col[i[1]];
		auto col_z = col[i[2]];

		t.setColor(0, col_x[0], col_x[1], col_x[2]);
		t.setColor(1, col_y[0], col_y[1], col_y[2]);
		t.setColor(2, col_z[0], col_z[1], col_z[2]);

		rasterize_triangle(t);
	}
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) 
{
	// TODO : Find out the bounding box of current triangle.
	// iterate through the pixel and find if the current pixel is inside the triangle

	// 构建BoundingBox包围盒，只考虑二维平面
	int MinX = static_cast<int>(std::min(t.v[0][0], std::min(t.v[1][0], t.v[2][0])));
	int MaxX = static_cast<int>(std::max(t.v[0][0], std::max(t.v[1][0], t.v[2][0])));
	int MinY = static_cast<int>(std::min(t.v[0][1], std::min(t.v[1][1], t.v[2][1])));
	int MaxY = static_cast<int>(std::max(t.v[0][1], std::max(t.v[1][1], t.v[2][1])));

	for (int x = MinX; x <= MaxX; ++x)
	{
		for (int y = MinY; y <= MaxY; ++y)
		{
			// 颜色的着色比例，默认覆盖像素中央时全着色
			float colorPercent = 1.f;
#if 0
			if (!t.insideTriangle(static_cast<float>(x + 0.5f), static_cast<float>(y + 0.5f))) continue;
			// 计算深度在三角形中的插值 
			float z_interpolated = calcDepth3D(x, y, t);
#elif 1
			// 将一个像素分成4个部分分别进行多重采样，取4个子采样点中深度插值最小的
			float z_interpolated = FLT_MAX;
			// 被三角形覆盖的子采样点的数里
			int coverCount = 0;

			// 从左上角开始，从左往右，从上往下处理
			if (t.insideTriangle(static_cast<float>(x + 0.25f), static_cast<float>(y + 0.75f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}
			if (t.insideTriangle(static_cast<float>(x + 0.75f), static_cast<float>(y + 0.75f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}
			if (t.insideTriangle(static_cast<float>(x + 0.25f), static_cast<float>(y + 0.25f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}
			if (t.insideTriangle(static_cast<float>(x + 0.75f), static_cast<float>(y + 0.25f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}

			if (coverCount == 0) continue;
			else colorPercent = coverCount / 4.f;
#elif 0
			// 将一个像素分成8个部分分别进行多重采样，取4个子采样点中深度插值最小的
			float z_interpolated = FLT_MAX;
			// 被三角形覆盖的子采样点的数里
			int coverCount = 0;

			// 从左上角开始，从左往右，从上往下处理
			if (t.insideTriangle(static_cast<float>(x + 0.125f), static_cast<float>(y + 0.875f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}
			if (t.insideTriangle(static_cast<float>(x + 0.875f), static_cast<float>(y + 0.875f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}
			if (t.insideTriangle(static_cast<float>(x + 0.25f), static_cast<float>(y + 0.75f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}
			if (t.insideTriangle(static_cast<float>(x + 0.75f), static_cast<float>(y + 0.75f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}
			if (t.insideTriangle(static_cast<float>(x + 0.25f), static_cast<float>(y + 0.25f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}
			if (t.insideTriangle(static_cast<float>(x + 0.75f), static_cast<float>(y + 0.25f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}
			if (t.insideTriangle(static_cast<float>(x + 0.125f), static_cast<float>(y + 0.125f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}
			if (t.insideTriangle(static_cast<float>(x + 0.875f), static_cast<float>(y + 0.125f)))
			{
				z_interpolated = std::min(z_interpolated, calcDepth3D(x, y, t));
				++coverCount;
			}

			if (coverCount == 0) continue;
			else colorPercent = coverCount / 8.f;
#endif

			// 与深度缓存进行比较
			int pointIdx = get_index(x, y);
			if (z_interpolated < depth_buf[pointIdx])
			{
				// 更新深度缓存
				depth_buf[pointIdx] = z_interpolated;

				// 重新设置像素点颜色
				Vector3f color = t.getColor() * colorPercent;
				Vector3f point;
				point << x, y, z_interpolated;
				set_pixel(point, color);
			}
		}
	}

	// If so, use the following code to get the interpolated z value.
	//auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
	//float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
	//float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
	//z_interpolated *= w_reciprocal;

	// TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
	model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
	view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
	projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
	if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
	{
		std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{ 0, 0, 0 });
	}
	if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
	{
		std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
	}
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
	frame_buf.resize(w * h);
	depth_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
	return (height - 1 - y) * width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
	//old index: auto ind = point.y() + point.x() * width;
	auto ind = (height - 1 - point.y()) * width + point.x();
	frame_buf[ind] = color;

}

// clang-format on