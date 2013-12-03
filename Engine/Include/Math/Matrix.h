#ifndef MATRIX_H
#define MATRIX_H

namespace Engine
{
	class Matrix
	{
	public:
		Matrix();
		Matrix(float a_input[16]);
		Matrix(float, float, float, float,
									 float, float, float, float,
									 float, float, float, float,
									 float, float, float, float);
		static Matrix Identity();
		static Matrix Translation(float a_x, float a_y, float a_z);
		float m[16];

		Matrix operator*(Matrix a_rhs);
		Matrix& operator*=(Matrix a_rhs);
	};
}

#endif // MATRIX_H