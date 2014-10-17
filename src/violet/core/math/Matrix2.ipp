#include <cmath>

template<typename T>
Matrix2<T>::Matrix2(const T rotation) {
	ul = std::cos(rotation);
	ur = -std::sin(rotation);
	dl = std::sin(rotation);
	dr = std::cos(rotation);
}
