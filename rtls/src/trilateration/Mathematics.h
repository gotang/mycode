#ifndef MATHEMATICS_H_
#define MATHEMATICS_H_

bool fequal(double x, double y);

struct Coordinate {
public:
	Coordinate()
		:x(0.0),
		 y(0.0),
		 z(0.0) {

	}
	Coordinate(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Coordinate(const Coordinate &c) {
		this->x = c.x;
		this->y = c.y;
		this->z = c.z;
	}
	void set(double x, double y, double z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Coordinate& operator=(const Coordinate &c) {
		this->x = c.x;
		this->y = c.y;
		this->z = c.z;
		return *this;
	}
	double x;
	double y;
	double z;
};

struct Sphere {
public:
	Sphere()
		:r(0.0) {
	}
	Sphere(const Coordinate &c, double r) {
		this->c = c;
		this->r = r;
	}
	Sphere(const Sphere &s) {
		this->c = s.c;
		this->r = s.r;
	}

	Sphere(double x, double y, double z, double r)
		:c(x, y, z),
		 r(r) {
	}
	void set(Coordinate& c, double r) {
		this->c = c;
		this->r = r;
	}

	Sphere& operator=(const Sphere &s) {
		r = s.r;
		c = s.c;
		return *this;
	}
	Coordinate c;
	double r;
private:
};

#endif//MATHEMATICS_H_
