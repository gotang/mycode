#ifndef DECAWAVE_TRILATERATION_H_
#define DECAWAVE_TRILATERATION_H_
struct vec3d {
	double	x;
	double	y;
	double	z;
};
int GetLocation(vec3d *solution, int use4thAnchor, vec3d* anchorArray, int *distanceArray);

#endif //DECAWAVE_TRILATERATION_H_
