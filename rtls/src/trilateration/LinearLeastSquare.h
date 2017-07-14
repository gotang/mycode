#ifndef TRILATERATION_H_
#define TRILATERATION_H_

#include "Mathematics.h"

#include <vector>
using namespace std;

template <typename T>
class Matrix {
public:
	Matrix(int M, int N);
	Matrix(T *data, int M, int N);
	Matrix(const Matrix<T> &matrix);
	~Matrix();
	//swap row i and j
	void swap(int i, int j);

	//This is not accurate when T is float.
	bool equal(const Matrix<T>& B) const;

	//return A^T
	Matrix<T> transpose() const;

	//return A + B
	Matrix<T> plus(const Matrix<T> &B) const;

	//return A - B
	Matrix<T> minus(const Matrix<T> &B) const;

	//return A * B
	Matrix<T> times(const Matrix<T> &B) const;

	//sovle Ax=b
	Matrix<T> solve(const Matrix<T> &rhs) const;
	//Matrix<T> inverse() const;
	T determinant() const;

	void show(const char *name = NULL) const;

	inline int rows() {
		return mRows;
	}
	inline int cols() {
		return mCols;
	}

	inline void setRows(int rows) {
		mRows = rows;
	}
	inline void setCols(int cols) {
		mCols = cols;
	}
	T& operator()(const size_t r, const size_t c) const {
		return mData[index(r, c)];
	}
	static Matrix<T> random(int M, int N);
	static Matrix<T> identity(int N);
private:
	size_t index(const size_t r, const size_t c) const {
		return r * mCols + c;
	}
	int mRows; // number of rows
	int mCols; // number of columns
	T *mData;
};
template class Matrix<double>;
template class Matrix<int>;
bool trilateration(vector<Sphere>& spheres, Coordinate& tag);

#endif //TRILATERATION_H_
