#ifndef MATRIX2_IPP
#define MATRIX2_IPP

#include <cmath>

using namespace Violet;

template <typename T>
const Matrix2<T> Matrix2<T>::Identity(1, 0, 0, 1);

template <typename T>
const Matrix2<T> Matrix2<T>::Zero;

template <typename T>
Matrix2<T>::Matrix2() :
	a(0),
	b(0),
	c(0),
	d(0)
{
}

template <typename T>
Matrix2<T>::Matrix2(T _a, T _b, T _c, T _d) :
	a(_a),
	b(_b),
	c(_c),
	d(_d)
{
}

template <typename T>
Matrix2<T>::Matrix2(const T rotation) {
	ul = std::cos(rotation);
	ur = -std::sin(rotation);
	dl = std::sin(rotation);
	dr = std::cos(rotation);
}

template <typename T>
Matrix2<T> Matrix2<T>::inverse() const
{
	const T dt = determinant();
	return dt != 0 ? Matrix2<T>(d, -b, -c, a) / dt : Identity;
}

template <typename T>
T Matrix2<T>::determinant() const
{
	return a * d - c * b;
}

template <typename T>
Matrix2<T> & Matrix2<T>::operator/=(const T scale)
{
	a /= scale;
	b /= scale;
	c /= scale;
	d /= scale;
	return *this;
}

template <typename T>
Vec2<T> Violet::operator*(const Matrix2<T> & mat, const Vec2<T> & vec)
{
	return Vec2<T>(vec.x * mat.a + vec.y * mat.b, vec.x * mat.c + vec.y * mat.d);
}

template <typename T>
Matrix2<T> Violet::operator/(Matrix2<T> mat, const T scale)
{
	return mat /= scale;
}

template <typename T>
bool Violet::operator==(const Matrix2<T> & lhs, const Matrix2<T> & rhs)
{
	return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c && lhs.d == rhs.d;
}

template <typename T>
std::ostream & Violet::operator<<(std::ostream & os, const Matrix2<T> & mat)
{
	return os << "|" << mat.a << "," << mat.b << "|" << mat.c << "," << mat.d << "|";
}

#endif
