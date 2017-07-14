#include <QCoreApplication>



/*
 * builder.cpp
 *
 *  Created on: Apr 21, 2017
 *      Author: sheldon
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <math.h>

//anchors lined up clockwise
//TA0(0.00,0.00) as origin
//A0->A1:57.51 A0->A2:120.34 A0->A3:104.27
//A1->A0:57.51 A1->A2:102.81 A1->A3:115.77

//
//assuming A0 as origin, and A1 is located on X-axis
//then calculate coordinate of other anchors

struct Point {
        double x;
        double y;
};

struct AnchorDistance{
        int id;
        double toA0;
        double toA1;
};

#if 0
const AnchorDistance anchors[] = {
                {0, 0.00, 57.51},//A0
                {1, 57.51, 0.00},//A1
                {2, 120.34, 104.27},//A2
                {3, 105.81, 115.77},//A3
};
#else
const AnchorDistance anchors[] = {
                {0, 0.00, 3},//A0
                {1, 3, 0.00},//A1
                {2, 5, 4},//A2
                {3, 9.43, 8.24},//A3
};
#endif

/*
 *(A0->x - x)^2 + (A0->y - y)^2 = dA0^2
 *(A1->x - x)^2 + (A1->y - y)^2 = dA1^2
 *A0(0.00, 0.00)
 *A1(A1->x,0.00)
*/
void getCoodinate(const Point& A0, const Point& A1, const double dA0, const double dA1, Point& x) {
        x.x = (dA0 * dA0 - dA1 * dA1 + A1.x * A1.x) /(2 * A1.x);
        x.y = sqrt(dA0 * dA0 - x.x * x.x);
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int anchorNum = sizeof(anchors)/sizeof(anchors[0]);
    Point *coord = new Point[anchorNum];
    coord[0].x = 0.00;
    coord[0].y = 0.00;

    coord[1].x = anchors[1].toA0;
    coord[1].y = 0.00;

    for(int i = 2; i < anchorNum; i ++) {
            getCoodinate(coord[0], coord[1], anchors[i].toA0, anchors[i].toA1, coord[i]);
    }
    for(int i = 0; i < anchorNum; i ++) {
            printf("A%02d(%.2f, %.2f)\n", i, coord[i].x, coord[i].y);
    }

    delete[] coord;
    return a.exec();
}
