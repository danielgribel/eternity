#ifndef Util_h
#define Util_h

#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>

using namespace std;

typedef vector<double> VD;
typedef vector<VD> VVD;
typedef vector<int> VI;

class Util {

	public:

		double MinCostMatching(const VVD &cost, VI &Lmate, VI &Rmate);

		vector<int> shuffle(size_t n);
		
		vector<int> shuffle(vector<int> vec);

};

#endif