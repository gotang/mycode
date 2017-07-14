#define LOG_NDEBUG 0
#define LOG_TAG "test"
#include "Log.h"

#include <math.h>
#include "gtest/gtest.h"

#include "DeviceManager.h"
#include "LinearLeastSquare.h"
#include "Trilateration.h"

static int array_0[4][2] = {
		 {1, 2},
		 {3, 9},
		 {4, 5},
		 {6, 1}
};

static int array_1[][4] = {
		{ 3,  1, -1,  2},
		{-5,  1,  3, -4},
		{ 2,  0,  1, -1},
		{ 1, -5,  3, -3}
};

static int array_2[][4] = {
		//swap row 1 and 2
		{ 3,  1, -1,  2},
		{ 2,  0,  1, -1},
		{-5,  1,  3, -4},
		{ 1, -5,  3, -3}
};

static int array_3[][4] = {
		//swap row 3 and 4
		{ 3,  1, -1,  2},
		{-5,  1,  3, -4},
		{ 1, -5,  3, -3},
		{ 2,  0,  1, -1}
};

static int array_4[][4] = {
		//1 + 2
		{ 6,   2, -2,  4},
		{-3,   1,  4, -5},
		{-3,   1,  4, -5},
		{ 2, -10,  6, -6}
};

static int array_5[][4] = {
		//1 - 2
		{ 0,  0,  0,  0},
		{-7,  1,  2, -3},
		{ 7, -1, -2,  3},
		{ 0,  0,  0,  0}
};

static int array_6[][2] = {
		//1 * 0
		{ 14,  12},
		{-14,  10},
		{  0,   8},
		{-20, -31}
};

static int array_7[][4] = {
		//1T
		{ 3, -5,  2,  1},
		{ 1,  1,  0, -5},
		{-1,  3,  1,  3},
		{ 2, -4, -1, -3}
};

static double array_8[][4] = {
		{ 3,  1, -1,  2},
		{-5,  1,  3, -4},
		{ 2,  0,  1, -1},
		{ 1, -5,  3, -3}
};

static double array_A1[][5] = {
		//A
		{0, 2, 3, 7, 5},// 0,
		{9, 2, 2, 8, 9},// 0,
		{7, 3, 6, 1, 2},// 0,
		{9, 3, 1, 9, 4},// 0,
		{7, 8, 4, 5, 0},// 0
		{7, 8, 4, 5, 0},// 0
};
//b
static double array_b1[] = { 3, 6, 1, 0, 6, 6};

static double array_A2[][3] = {
		//A
		{4,  3, 1},// 0,
		{1, -2, 3},// 0,
		{0, 0, 0},
		{5,  7, 0}
};
//b
static double array_b2[] = {35, 6, 0, 0};
static double array_x2[] = {7, 3, 2};

#define ROW(array) (sizeof(array)/sizeof(array[0]))
#define COL(array) (sizeof(array[0])/sizeof(array[0][0]))

static const Matrix<int> m0(&array_0[0][0], ROW(array_0), COL(array_0));
static const Matrix<int> m1(&array_1[0][0], ROW(array_1), COL(array_1));
static const Matrix<int> m2(&array_2[0][0], ROW(array_2), COL(array_2));
static const Matrix<int> m3(&array_3[0][0], ROW(array_3), COL(array_3));
static const Matrix<int> m4(&array_4[0][0], ROW(array_4), COL(array_4));
static const Matrix<int> m5(&array_5[0][0], ROW(array_5), COL(array_5));
static const Matrix<int> m6(&array_6[0][0], ROW(array_6), COL(array_6));
static const Matrix<int> m7(&array_7[0][0], ROW(array_7), COL(array_7));
static const Matrix<double> m8(&array_8[0][0], ROW(array_8), COL(array_8));

static const Matrix<double> A1(&array_A1[0][0], ROW(array_A1), COL(array_A1));
static const Matrix<double> b1(&array_b1[0], ROW(array_b1), 1);

static const Matrix<double> A2(&array_A2[0][0], ROW(array_A2), COL(array_A2));
static const Matrix<double> b2(&array_b2[0], ROW(array_b2), 1);

TEST(MatrixTest, Equal) {
	EXPECT_TRUE(m1.equal(m1));
	EXPECT_FALSE(m1.equal(m2));
}

TEST(MatrixTest, Swap) {
	Matrix<int> m1_l(&array_1[0][0], 4, 4);
	m1_l.swap(1, 2);
	EXPECT_TRUE(m1_l.equal(m1_l));
}

TEST(MatrixTest, Minus) {
	EXPECT_TRUE(m1.minus(m2).equal(m5));
}

TEST(MatrixTest, Plus) {
	EXPECT_TRUE(m1.plus(m2).equal(m4));
}

TEST(MatrixTest, Times) {
	EXPECT_TRUE(m1.times(m0).equal(m6));
}

TEST(MatrixTest, Transpose) {
	EXPECT_TRUE(m1.transpose().equal(m7));
}

TEST(MatrixTest, Inverse) {
	//m1.inverse().show();
}

TEST(MatrixTest, Determinant) {
	double d = m8.determinant();
	EXPECT_LE(fabs(d-40.0f), 1e-6);
}

TEST(MatrixTest, Solve) {
#if 0
	Matrix<double> t = A1.transpose();
	Matrix<double> E = t.times(A1);
	Matrix<double> B = t.times(b1);

	Matrix<double> x = E.solve(B);
	A1.show("A");
	t.show("A^T");
	E.show("E=A^T x A");
	B.show("B=A^T x b");
	x.show("x");
	A1.times(x).show("A x x");
	b1.show("b");
	EXPECT_TRUE(A1.times(x).equal(b1));
#endif
	Matrix<double> t = A2.transpose();
	Matrix<double> E = t.times(A2);
	Matrix<double> B = t.times(b2);

	Matrix<double> x = E.solve(B);
//	A2.show("A");
//	t.show("A^T");
//	E.show("E=A^T x A");
//	B.show("B=A^T x b");
//	x.show("x");
//	A2.times(x).show("A x x");
//	b2.show("b");
	EXPECT_TRUE(A2.times(x).equal(b2));

}

TEST(TrilaterationTest, Trilateraion) {
	Coordinate c(0, 0, 0);
	vector<Sphere> spheres;

	Sphere sphere(c, 5);
	spheres.push_back(sphere);

	c.set(3, 0, 0);
	sphere.set(c, 2);
	spheres.push_back(sphere);

	c.set(0, 4, 0);
	sphere.set(c, 2);
	spheres.push_back(sphere);

	Coordinate result(3, 4, 0);
	Coordinate tag;
	EXPECT_TRUE(trilateration(spheres, tag));
	EXPECT_TRUE(fequal(result.x, tag.x));
	EXPECT_TRUE(fequal(result.y, tag.y));
	EXPECT_TRUE(fequal(result.z, tag.z));
	ALOGV("x:%f, y:%f, z:%f", tag.x, tag.y, tag.z);
}

TEST(DecaWaveTrilateration, Trilateration) {
	vec3d solution;
	vec3d anchors[3];
	int distance[4] = {5, 2, 2};
	anchors[0].x = 0;
	anchors[0].y = 0;
	anchors[0].z = 0;

	anchors[1].x = 3;
	anchors[1].y = 0;
	anchors[1].z = 0;

	anchors[2].x = 0;
	anchors[2].y = 4;
	anchors[2].z = 0;
	int err = GetLocation(&solution, 0, &anchors[0], &distance[0]);
	EXPECT_EQ(err, 3);
	printf("solution:(%.2f, %.2f, %.2f)\n", solution.x, solution.y, solution.z);
}

TEST(DeviceManagerTest, Locate) {
	DeviceManager manager;

	Device device;
	vector<Device> anchors;
#if 0
	int id = 0;
	device.mId = id ++;
	device.mCoord.set(0, 0, 0);;
	anchors.push_back(device);

	device.mId = id ++;
	device.mCoord.set(3, 0, 0);
	anchors.push_back(device);

	manager.updateAnchors(anchors);

	device.mId = id ++;
	device.mCoord.set(0, 4, 0);
	manager.addDevice(TYPE_ANCHOR, device);
	manager.dump();

	vector<Distance> distances;
	Distance dist;

	dist.mId = 0;
	dist.mDist = 5;
	distances.push_back(dist);

	dist.mId = 1;
	dist.mDist = 5;
	distances.push_back(dist);

	dist.mId = 2;
	dist.mDist = 3;
	distances.push_back(dist);

	int tagid = 0;
	manager.updateTagPosition(tagid, distances);
	manager.dump();
	Coordinate c;
	EXPECT_TRUE(manager.getTagPosition(tagid, c));

	Coordinate result(3, 4, 0);
	EXPECT_TRUE(fequal(result.x, c.x));
	EXPECT_TRUE(fequal(result.y, c.y));
	EXPECT_TRUE(fequal(result.z, c.z));
#else
	int id = 0;
	device.mId = id ++;
	device.mCoord.set(1, 2, 0);;
	anchors.push_back(device);

	device.mId = id ++;
	device.mCoord.set(3, 1, 0);
	anchors.push_back(device);

	manager.updateAnchors(anchors);

	device.mId = id ++;
	device.mCoord.set(2, 5, 0);
	manager.addDevice(TYPE_ANCHOR, device);
	manager.dump();

	vector<Distance> distances;
	Distance dist;

	dist.mId = 0;
	dist.mDist = 5;
	distances.push_back(dist);

	dist.mId = 1;
	dist.mDist = 3;
	distances.push_back(dist);

	dist.mId = 2;
	dist.mDist = 4;
	distances.push_back(dist);

	int tagid = 0;
	manager.updateTagPosition(tagid, distances);
	manager.dump();
	Coordinate c;
	EXPECT_TRUE(manager.getTagPosition(tagid, c));
#endif
}

extern void woxuTest();
TEST(WoXuTest, Locate) {
	woxuTest();
}

int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
