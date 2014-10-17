#ifndef MATRIX_H
#define MATRIX_H

template<typename T>
class Matrix2
{
public:

	explicit Matrix2(T rotation);

public:

	T ul, ur, dl, dr;
};

#endif
