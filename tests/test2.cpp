
#include <stdio.h>
#include <cassert>
#include <cstring>
#include <initializer_list>
#include <iostream>
using namespace std;
#define VAL_COUNT 4
struct A {
	A() {
		for (int j = 0; j < VAL_COUNT; j++) {
			val[j] = 0;
		}
	}

	A(const std::initializer_list<int> &list){
		int count = 0;
		for(auto &element : list) {
			if(count > VAL_COUNT)
				break;

			val[count++] = element;
		}
	}
	void clear() {
		for (int i = 0; i < VAL_COUNT; i++) {
			val[i] = 0;
		}
	}
	A& operator+=(const A &a) {
		for(int j = 0; j < VAL_COUNT; j ++) {
			val[j] += a.val[j];
		}
		return *this;
	}

	A& operator=(const A &a) {
		for(int j = 0; j < VAL_COUNT; j ++) {
			val[j] = a.val[j];
		}
		return *this;
	}

	int p1() {
		return val[3] - val[2];
	}

	int p2() {
		return val[3] - val[0];
	}

	int l() {
		return val[2];
	}

	void dump() {
		for(int j = 0; j < VAL_COUNT; j ++) {
			printf("%d ", val[j]);
		}
		printf("%d \n", val[3] - val[2]);
	}

	int val[VAL_COUNT];
};
struct MapA {
	A sum;
	A *addr;
	int size;
	int m;
};

#if 0
A _0[] = {
		{600, 180, 420, 1200},
		{600, 180, 420, 1200},
		{600, 180, 420, 1200},
		{600, 180, 420, 1200},

		{150, 45, 105, 350},
		{130, 52, 78, 300},
		{150, 45, 105, 350},
		{150, 45, 105, 350},
};
A _1[] = {
		{5000, 1500, 3500, 10000},

		{1200, 180, 420, 1200},
		{1200, 180, 420, 1200},
		{350, 45, 105, 350},
		{300, 52, 78, 300},

		{600, 180, 420, 600},
};
A _2[] = {
		{4000, 3000, 1000, 6000},

		{1200, 180, 420, 1200},
		{1200, 180, 420, 1200},
		{350, 45, 105, 350},
		{300, 52, 78, 300},
		{600, 180, 420, 600},
};

A _3[] = {
		{1200, 1200, 0, 1200},
		{350, 350, 0, 350},
		{300, 300, 0, 300},
		{600, 600, 0, 600},
		{300, 300, 0, 300}
};

A _4[] = {
		{600, 600, 0, 600},
		{600, 600, 0, 600},
		{320, 320, 0, 320},
		{300, 300, 0, 300}
};

MapA map[] = {
		{A(), _0, sizeof(_0)/sizeof(_0[0]), 13},
		{A(), _1, sizeof(_1)/sizeof(_1[0]), 6},
		{A(), _2, sizeof(_2)/sizeof(_2[0]), 7},
		{A(), _3, sizeof(_3)/sizeof(_3[0]), 0},
		{A(), _4, sizeof(_4)/sizeof(_4[0]), 0},
};
#else
A _0[] = {
		{1000, 1000, 0, 1000},
};

A _1[] = {
		{600, 180, 420, 1200},
		{600, 180, 420, 1200},
		{600, 180, 420, 1200},
		{600, 180, 420, 1200},

		{150, 45, 105, 350},
		{130, 52, 78, 300},
		{150, 45, 105, 350},
		{150, 45, 105, 350},
};
A _2[] = {
		{350, 45, 105, 350},
		{300, 52, 78, 300},
		{600, 200, 400, 600},
};

A _3[] = {
		{600, 600, 0, 600},
		{600, 600, 0, 600},
		{320, 320, 0, 320},
		{350, 350, 0, 350},
};

//500 - 200 -100 = 200

//150 * 10 = 1500
//50 - 30 = 20
//1500 + 20 + 300 - 78 - 105 - 400 - 600 - 320 = 317

//1000(150)==>600
//2800(400)==>600
//(600+600) * 2000/70 = 34,000

MapA map[] = {
		{A(), _0, sizeof(_0)/sizeof(_0[0]), 0},
		{A(), _1, sizeof(_1)/sizeof(_1[0]), 13},
		{A(), _2, sizeof(_2)/sizeof(_2[0]), 12},
		{A(), _3, sizeof(_3)/sizeof(_3[0]), 7},
};
#endif

int main() {

	float l = 0;
	float remain = 0;

	for(int i = 0; i < sizeof(map)/sizeof(map[0]); i ++) {
		for(int j = 0; j < map[i].size; j ++) {
			map[i].sum += map[i].addr[j];
		}

		map[i].sum.dump();
		if(i < 1)
			continue;

		l = map[i].sum.l()/100 * 0.5 * map[i].m;
		remain += map[i - 1].sum.p1() - (map[i].sum.p1() - map[i].sum.p2());
		remain -=l;

		cout << "step " << i << " ==> [p1 " << map[i].sum.p1() <<
				", p2 " << map[i].sum.p2() << ", l " << l <<
				", remain " << remain << "]" << endl << endl;
	}
	return 0;
}
