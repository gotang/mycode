//#define LOG_NDEBUG 0
#define LOG_TAG "LinearLeastSquare"
#include "Log.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <typeinfo>

#include "Mathematics.h"
#include "LinearLeastSquare.h"

// create M-by-N matrix of 0's
template <typename T>
Matrix<T>::Matrix(int M, int N) {
	mRows = M;
	mCols = N;
	mData = new T [M * N];
	memset(mData, 0, sizeof(T) * M * N);
}

// create matrix based on 2d array
template <typename T>
Matrix<T>::Matrix(T *data, int M, int N) {
	mRows = M;
	mCols = N;
	mData = new T[M * N];
	//do not copy mRows * mCols * sizeof(T) directly from data
	//because memory of data is larger than mRows * mCols * sizeof(T)
	for (int i = 0; i < mRows; i++) {
		memcpy(mData + i * mCols, data + i * mCols, mCols * sizeof(T));
	}
}

template <typename T>
Matrix<T>::Matrix(const Matrix<T>& matrix) {
	mRows = matrix.mRows;
	mCols = matrix.mCols;

	mData = new T[mRows * mCols];
	for (int i = 0; i < mRows; i++) {
		memcpy(mData + i * mCols, matrix.mData + i * mCols, mCols * sizeof(T));
	}
}

template <typename T>
Matrix<T>::~Matrix() {
	if (mData != NULL) {
		delete[] mData;
		mData = NULL;
	}
}

// create and return a random M-by-N matrix with values between 0 and 1
//static
template <typename T>
Matrix<T> Matrix<T>::random(int M, int N) {
	Matrix<T> A(M, N);
	for (int i = 0; i < M; i++)
		for (int j = 0; j < N; j++)
			A(i, j) = ::random()%10;
	return A;
}

// create and return the N-by-N identity matrix
//static
template <typename T>
Matrix<T> Matrix<T>::identity(int N) {
	Matrix<T> A(N, N);
	for (int i = 0; i < N; i++)
		A(i, i) = 1;
	return A;
}

// swap rows i and j
template <typename T>
void Matrix<T>::swap(int i, int j) {
	T *temp = new T[mRows];
	memcpy(&temp[0], mData + i * mCols, mCols * sizeof(T));
	memcpy(mData + i * mCols, mData + j * mCols, mCols * sizeof(T));
	memcpy(mData + j * mCols, &temp[0], mCols * sizeof(T));
	delete[] temp;
}

// create and return the transpose of the invoking matrix
template <typename T>
Matrix<T> Matrix<T>::transpose() const {
	Matrix<T> A(mCols, mRows);

	for (int i = 0; i < mRows; i++)
		for (int j = 0; j < mCols; j++)
			A(j, i) = operator()(i, j);
	return A;
}

// return C = A + B
template <typename T>
Matrix<T> Matrix<T>::plus(const Matrix<T> &B) const {
	ALOGV("plus");
	Matrix<T> C(mRows, mCols);
	if (B.mCols != mCols || B.mRows != mRows) {
		ALOGE("executing plus with matrix[%d][%d] and matrix[%d][%d]", mRows, mCols, B.mRows, B.mCols);
		TRESPASS();
		return C;
	}

	for (int i = 0; i < mRows; i++)
		for (int j = 0; j < mCols; j++)
			C(i, j) = operator()(i, j) + B(i, j);
	return C;
}

// return C = A - B
template <typename T>
Matrix<T> Matrix<T>::minus(const Matrix<T> &B) const {
	Matrix<T> C(mRows, mCols);
	if (B.mCols != mCols || B.mRows != mRows) {
		ALOGE("executing minus with matrix[%d][%d] and matrix[%d][%d]", mRows, mCols, B.mRows, B.mCols);
		TRESPASS();
		return C;
	}

	for (int i = 0; i < mRows; i++)
		for (int j = 0; j < mCols; j++)
			C(i, j) = operator()(i, j) - B(i, j);
	return C;
}


// return C = A * B
template <typename T>
Matrix<T> Matrix<T>::times(const Matrix<T> &B) const {
	Matrix<T> C(mRows, B.mCols);
	if (mCols != B.mRows) {
		ALOGE("executing times with matrix[%d][%d] and matrix[%d][%d]", mRows, mCols, B.mRows, B.mCols);
		TRESPASS();
		return C;
	}
	for (int i = 0; i < C.mRows; i++) {
		for (int j = 0; j < C.mCols; j++) {
			for (int k = 0; k < mCols; k++) {
				C(i, j) += operator()(i, k) * B(k, j);
			}
		}
	}
	return C;
}

// return x = A^-1 b, assuming A is square and has full rank
template <typename T>
Matrix<T> Matrix<T>::solve(const Matrix<T> &rhs) const{
	Matrix<T> x(mCols, 1);
	if (mRows != mCols || mCols != rhs.mRows || rhs.mCols != 1) {
		ALOGE("executing solve with matrix[%d][%d] and matrix[%d][%d]", mRows, mCols, rhs.mRows, rhs.mCols);
		TRESPASS();
		return x;
	}
    // create copies of the data
    Matrix<T> A(*this);
    Matrix<T> b(rhs);

    // Gaussian elimination with partial pivoting
    for (int i = 0; i < mRows; i++) {
        // find pivot row and swap
        int max = i;
        for (int j = i + 1; j < mRows; j++)
            if (abs(A(j, i)) > abs(A(max, i)))
                max = j;

        A.swap(i, max);
        b.swap(i, max);
        // singular
        if (fequal(A(i, i), 0.0f)) {
    		ALOGE("Matrix is singular");
    		return x;
        }
        // pivot within b
        for (int j = i + 1; j < mRows; j++)
        	b(j, 0) -= b(i, 0) * A(j, i)/A(i, i);

        // pivot within A
        for (int j = i + 1; j < mRows; j++) {
        	T m = A(j, i)/A(i, i);
            for (int k = i + 1; k < mCols; k++) {
            	A(j, k) -= A(i, k) * m;
            }
            A(j, i) = (T)0;
        }
    }
    for (int j = mRows - 1; j >= 0; j--) {
        T t = 0.0;
        for (int k = j + 1; k < mRows; k++)
        	t += A(j, k) * x(k, 0);
        x(j, 0) = (b(j, 0) - t)/A(j, j);
    }
    return x;
}

// does A = B exactly?
template <typename T>
bool Matrix<T>::equal(const Matrix<T> &B) const {
	if (B.mCols != mCols || B.mRows != mRows) {
		ALOGE("executing minus equal matrix[%d][%d] and matrix[%d][%d]", mRows, mCols, B.mRows, B.mCols);
		return false;
	}
	if(typeid(T) == typeid(float) || typeid(T) == typeid(double)) {
		//never compare float types implicitly.
		for (int i = 0; i < mRows; i++)
			for (int j = 0; j < mCols; j++)
				if(!fequal(operator()(i, j), B(i, j)))
					return false;
	} else {
		for (int i = 0; i < mRows; i++)
			for (int j = 0; j < mCols; j++)
				if(operator()(i, j) != B(i, j))
					return false;
	}

	return true;
}

//print matrix to standard output
template <typename T>
void Matrix<T>::show(const char *name) const {
	if(name)
		ALOGV("show matrix %s:", name);
	int size = 15 * mCols;
	for (int i = 0; i < mRows; i++) {
		for (int j = 0; j < mCols; j++) {
			printf("%8.2f ", (double)(operator()(i,j)));
		}
		printf("\n");
	}
	printf("\n");
}

template <typename T>
T Matrix<T>::determinant() const {
	Matrix<T> x(mCols, 1);
	if (mRows != mCols) {
		ALOGE("executing determinant with matrix[%d][%d]", mRows, mCols);
		return -1;
	}

	T sum = 1;

	// create copies of the data
	Matrix<T> A(*this);

	// Gaussian elimination with partial pivoting
	for (int i = 0; i < mRows; i++) {
		// find pivot row and swap
		int max = i;
		for (int j = i + 1; j < mRows; j++)
			if (abs(A(j, i)) > abs(A(max, i)))
				max = j;

		A.swap(i, max);

		// singular
		if (A(i, i) == 0) {
			ALOGE("Matrix is singular");
			sum = 0;
			break;
		}

		// pivot within A
		for (int j = i + 1; j < mRows; j++) {
			T m = A(j, i) / A(i, i);
			for (int k = i + 1; k < mCols; k++) {
				A(j, k) -= A(i, k) * m;
			}
			A(j, i) = (T) 0;
		}
	}
	for (int i = 0; i < mRows; i++) {
		sum *= A(i, i);
	}
	return sum;
}

///////////////////////////////////////////////////////////////////////////////////////////////
/*
 * This is to sovle the matrix equation Ax = b with
 *     _                         _
 *     | x2 - x1 y2 - y1 z2 - z1 |
 * A = | x3 - x1 y3 - y1 z3 - z1 |
 *     |    .       .       .    |
 *     |    .       .       .    |
 *     |    .       .       .    |
 *     | xn - x1 yn - y1 zn - z1 |
 *     _                         _
 *
 *     _        _
 *     | x - x1 |
 * x = | y - y1 |
 *     | z - z1 |
 *     _        _
 *
 *     _     _
 *     | b21 |
 * b = | b31 |
 *     |  .  |
 *     |  .  |
 *     |  .  |
 *     | bn1 |
 *     _     _
 *
 *  where
 *  bn1 = (r1^2 - rn^2 + (xn - x1)^2 + (yn - y1)^2 + (zn - z1)^2)/2
 *
 *  since there're (n - 1) equations and 3 unkowns, therefore theoretically
 *  only four anchors are needed to determine the unique position of a tag.
 *
 */

void dump(vector<Sphere>& spheres) {
	ALOGV("dump spheres:");
	for(size_t i = 0; i < spheres.size(); i ++) {
		ALOGV("%8.2f %8.2f %8.2f %8.2f", spheres[i].c.x, spheres[i].c.y, spheres[i].c.z, spheres[i].r);
	}
}

bool trilateration(vector<Sphere>& spheres, Coordinate& tag) {
	if (spheres.size() < 3) {
		//at least 3 equations are needed to position 2-D tag.
		ALOGE("sphere size is %zd", spheres.size());
		return false;
	}
	dump(spheres);

	int cols = 2;
	for(size_t i = 1; i < spheres.size(); i ++) {
		if (spheres[i].c.z != spheres[0].c.z) {
			cols = 3;
			break;
		}
	}

	int rows = spheres.size() - 1;
	ALOGV("rows %d, cols %d", rows, cols);
	//matrix A
	Matrix<double> A(rows, cols);
	for(int i = 1; i < rows + 1; i ++) {
		A(i - 1, 0) = spheres[i].c.x - spheres[0].c.x;
		A(i - 1, 1) = spheres[i].c.y - spheres[0].c.y;
		if(cols == 3) {
			A(i - 1, 2) = spheres[i].c.z - spheres[0].c.z;
		}
	}

	//matrix b
	Matrix<double> b(rows, 1);
	for(int i = 1; i < rows + 1; i ++) {
		b(i - 1, 0) = (pow(spheres[0].r, 2) - pow(spheres[i].r, 2) + pow(spheres[i].c.x - spheres[0].c.x, 2)
				+ pow(spheres[i].c.y - spheres[0].c.y, 2) + pow(spheres[i].c.z - spheres[0].c.z, 2))/2;
	}

	Matrix<double> AT = A.transpose();
	Matrix<double> E = AT.times(A);
	Matrix<double> B = AT.times(b);
	Matrix<double> x = E.solve(B);
#if 0
	A.show("A");
	b.show("b");
	AT.show("A^T");
	E.show("E=A^T x A");
	B.show("B=A^T x b");
	x.show("x");
#endif
	//return result
	tag.x = x(0, 0) - spheres[0].c.x;
	tag.y = x(1, 0) - spheres[0].c.y;
	if(cols == 3) {
		tag.z = x(2, 0) - spheres[0].c.z;
	} else {
		tag.z = 0;
	}

	return true;
}

