#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <string>
#include <map>
#include <ctime>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>
#include <limits>
#include <queue>
#include "Solution.h"

using namespace std;

#define InputFile "../set1/pieces_set_1/pieces_04x04.txt"

vector< vector<int> > load(string dataFileName) {
    // Open the data file
    ifstream input(dataFileName.c_str());

    if (! input) {
        cerr << "Unable to open data file: " << dataFileName << endl;
    }
    
    // Read the size of the puzzle
    int n, m;
    input >> n;
    input >> m;
    
    // The puzzle has n*m pieces and each piece has 4 patterns
    vector< vector<int> > pieces (n * m, vector<int>(4));

    // Read the data values directly into the vector of pieces
    for (int i = 0; i < n * m; i++) {
        input >> pieces[i][0];
        input >> pieces[i][1];
        input >> pieces[i][2];
        input >> pieces[i][3];
    }

    return pieces;
}

int main() {
	vector< vector<int> > pieces = load(InputFile); // Load the vector of pieces
	Solution * solution = new Solution(pieces);
	vector<int> assignment = solution->getAssignment();
	vector<int> rotation = solution->getRotation();
	vector< vector <int> > m = solution->mapAssignment();

	// for (int i = 0; i < m.size(); i++) {
	// 	cout << m[i][0] << ", " << m[i][1] << ", " << m[i][2] << endl;
	// }

	solution->rotate(3, 2);
	solution->rotate(7, 2);
	solution->rotate(13, 3);
	solution->swap(6, 9);
	
	//solution->computeCost();

	cout << solution->getCost() << endl;

	return 0;
}