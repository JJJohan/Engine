#include "stdafx.h"

namespace Engine
{
	Matrix::Matrix()
	{
		for (int i = 0; i < 16; ++i)
			m[i] = 0.0f;
	}

	Matrix::Matrix(float a_input[16])
	{
		for (int i = 0; i < 16; ++i)
			m[i] = a_input[i];
	}

	Matrix::Matrix(float m1, float m2, float m3, float m4,
					   float m5, float m6, float m7, float m8,
					   float m9, float m10, float m11, float m12,
					   float m13, float m14, float m15, float m16)
	{
		m[0] = m1;
		m[1] = m2;
		m[2] = m3;
		m[3] = m4;
		m[4] = m5;
		m[5] = m6;
		m[6] = m7;
		m[7] = m8;
		m[8] = m9;
		m[9] = m10;
		m[10] = m11;
		m[11] = m12;
		m[12] = m13;
		m[13] = m14;
		m[14] = m15;
		m[15] = m16;
	}

	Matrix Matrix::Identity()
	{
		return Matrix(1.0f, 0.0f, 0.0f, 0.0f,
					    0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f);
	}

	Matrix Matrix::Translation(float a_x, float a_y, float a_z)
	{
		return Matrix(1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						a_x, a_y, a_z, 1.0f);
	}

	Matrix Matrix::operator*(Matrix a_rhs)
	{
		return Matrix(m[0] * a_rhs.m[0], m[1] * a_rhs.m[1], m[2] * a_rhs.m[2], m[3] * a_rhs.m[3],
						m[4] * a_rhs.m[4], m[5] * a_rhs.m[5], m[6] * a_rhs.m[6], m[7] * a_rhs.m[7],
						m[8] * a_rhs.m[8], m[9] * a_rhs.m[9], m[10] * a_rhs.m[10], m[11] * a_rhs.m[11],
						m[12] * a_rhs.m[12], m[13] * a_rhs.m[13], m[14] * a_rhs.m[14], m[15] * a_rhs.m[15]);
	}

	Matrix& Matrix::operator*=(Matrix a_rhs)
	{
		for (int i = 0; i < 16; ++i)
			m[i] = a_rhs.m[i];

		return *this;
	}
}