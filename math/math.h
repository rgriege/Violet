#ifndef VIOLET_MATH_H
#define VIOLET_MATH_H

inline int max(int lhs, int rhs)
{
	return lhs < rhs ? rhs : lhs;
}

inline int min(int lhs, int rhs)
{
	return lhs > rhs ? rhs : lhs;
}

#define PI 3.1415

#endif
