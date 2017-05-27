#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

struct LocalSolutionWrapper {
	int region;
	int error;
    vector<int> rotation;
    vector<int> permutation;
};