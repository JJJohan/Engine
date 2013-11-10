#ifndef MATRIX_H
#define MATRIX_H

namespace Engine
{
	class Matrix
	{
	public:
		__declspec(dllexport) Matrix();
		__declspec(dllexport) Matrix(float a_input[16]);
		__declspec(dllexport) Matrix(float, float, float, float,
									 float, float, float, float,
									 float, float, float, float,
									 float, float, float, float);
		__declspec(dllexport) static Matrix Identity();
		__declspec(dllexport) static Matrix Translation(float a_x, float a_y, float a_z);
		float m[16];

		__declspec(dllexport) Matrix operator*(Matrix a_rhs);
		__declspec(dllexport) Matrix& operator*=(Matrix a_rhs);
	};
}

#endif // MATRIX_H