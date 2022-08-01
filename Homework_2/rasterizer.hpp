//
// Created by goksu on 4/6/19.
//

#pragma once

#include <Eigen/Eigen>
#include <algorithm>
#include "global.hpp"
#include "Triangle.hpp"
using namespace Eigen;

namespace rst
{
	enum class Buffers
	{
		Color = 1,
		Depth = 2
	};

	inline Buffers operator|(Buffers a, Buffers b)
	{
		return Buffers((int)a | (int)b);
	}

	inline Buffers operator&(Buffers a, Buffers b)
	{
		return Buffers((int)a & (int)b);
	}

	enum class Primitive
	{
		Line,
		Triangle
	};

	/*
	 * For the curious : The draw function takes two buffer id's as its arguments. These two structs
	 * make sure that if you mix up with their orders, the compiler won't compile it.
	 * Aka : Type safety
	 * */
	struct pos_buf_id
	{
		int pos_id = 0;
	};

	struct ind_buf_id
	{
		int ind_id = 0;
	};

	struct col_buf_id
	{
		int col_id = 0;
	};

	class rasterizer
	{
	public:
		rasterizer(int w, int h);
		pos_buf_id load_positions(const std::vector<Eigen::Vector3f>& positions);
		ind_buf_id load_indices(const std::vector<Eigen::Vector3i>& indices);
		col_buf_id load_colors(const std::vector<Eigen::Vector3f>& colors);

		void set_model(const Eigen::Matrix4f& m);
		void set_view(const Eigen::Matrix4f& v);
		void set_projection(const Eigen::Matrix4f& p);

		void set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color);

		void clear(Buffers buff);

		void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type);

		std::vector<Eigen::Vector3f>& frame_buffer() { return frame_buf; }

	private:
		void draw_line(Eigen::Vector3f begin, Eigen::Vector3f end);

		void rasterize_triangle(const Triangle& t);

		// VERTEX SHADER -> MVP -> Clipping -> /.W -> VIEWPORT -> DRAWLINE/DRAWTRI -> FRAGSHADER

		/**
		 * @brief 利用三角形重心坐标的插值结果，计算深度插值
		 *
		 */
		static inline float calcDepth3D(float x, float y, const Triangle& t)
		{
			// 插值计算深度，下述计算插值的代码由框架提供 
			std::tuple<float, float, float> ob = computeBarycentric2D(static_cast<float>(x + 0.5f), static_cast<float>(y + 0.5f), t.v);
			float alpha = std::get<0>(ob), beta = std::get<1>(ob), gamma = std::get<2>(ob);

			std::array<Vector4f, 3> v = t.toVector4();

			// 这一步本质上是计算在三维空间中的深度值 
			float depth3D = 1.f / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
			/**
			 * 这一步就相当于各个顶点的Z轴坐标值分别与重心坐标计算所得系数相乘相加，
			 * 即：α*az + β*bz + γ*cz（齐次坐标值通过toVector4接口获取，默认值为1.f），
			 * 以计算深度相对于三角形各个顶点的重心坐标插值
			 */
			float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();

			return z_interpolated * depth3D;
		}

		/**
		 * @brief 计算三角形的重心坐标系数
		 * @param x
		 * @param y
		 * @return 返回计算的系数α，β和γ的值
		 */
		static inline std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
		{
			float alpha = (
				x * (v[1].y() - v[2].y()) +
				(v[2].x() - v[1].x()) * y +
				v[1].x() * v[2].y() - v[2].x() * v[1].y()
				) / (
					v[0].x() * (v[1].y() - v[2].y()) +
					(v[2].x() - v[1].x()) * v[0].y() +
					v[1].x() * v[2].y() - v[2].x() * v[1].y()
					);

			float beta = (
				x * (v[2].y() - v[0].y()) +
				(v[0].x() - v[2].x()) * y +
				v[2].x() * v[0].y() - v[0].x() * v[2].y()
				) / (
					v[1].x() * (v[2].y() - v[0].y()) +
					(v[0].x() - v[2].x()) * v[1].y() +
					v[2].x() * v[0].y() - v[0].x() * v[2].y()
					);
			//float gamma = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());

			// 已求出α和β的情况下利用 α+β+γ=1，可直接计算出γ的值 
			float gamma = 1 - alpha - beta;

			return { alpha, beta, gamma };
		}

	private:
		Eigen::Matrix4f model;
		Eigen::Matrix4f view;
		Eigen::Matrix4f projection;

		std::map<int, std::vector<Eigen::Vector3f>> pos_buf;
		std::map<int, std::vector<Eigen::Vector3i>> ind_buf;
		std::map<int, std::vector<Eigen::Vector3f>> col_buf;

		std::vector<Eigen::Vector3f> frame_buf;

		std::vector<float> depth_buf;
		int get_index(int x, int y);

		int width, height;

		int next_id = 0;
		int get_next_id() { return next_id++; }
	};
}
