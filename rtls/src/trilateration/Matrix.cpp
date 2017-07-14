#define LOG_NDEBUG 0
#define LOG_TAG "Matrix"
#include "Log.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <typeinfo>

#include "Mathematics.h"
#include "Matrix.h"

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
