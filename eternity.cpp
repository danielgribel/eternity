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

#define InputFile "../set1/pieces_set_1/pieces_16x16.txt"

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

vector<int> shuffle(size_t n) {
    vector<int> myArray(n);
    for(int i = 0; i < n; i++) {
        myArray[i] = i;
    }
    if(n > 1) {
        size_t i;
        for (int i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = myArray[j];
            myArray[j] = myArray[i];
            myArray[i] = t;
        }
    }
    return myArray;
}

void solve() {
	vector< vector<int> > pieces = load(InputFile); // Load the vector of pieces
	Solution * solution = new Solution(pieces);
	vector<int> shufflePieces;

	// cout << "cost = " << solution->getCost() << endl;

	int i, delta;
	int it = 0;

	while(it < 100) {
		shufflePieces = shuffle(solution->getN());
		for (int i1 = 0; i1 < solution->getN(); i1++) {
			i = shufflePieces[i1];
			for (int j1 = 0; j1 < solution->getN(); j1++) {
				if(i != j1) {
					delta = solution->evalSwap(i, j1);
					if(delta < 1) {
						solution->swap(i, j1);
						solution->updateCost(delta);
					}	
				}
			}
			for (int j = 0; j < 4; j++) {
				delta = solution->evalRotate(i, j);
				if(delta < 1) {
					solution->rotate(i, j);
					solution->updateCost(delta);
				}
			}
		}
		it++;
	}

	// solution->swap(6, 5);
	// solution->swap(6, 10);
	// solution->swap(8, 4);
	// solution->swap(1, 2);
	// solution->swap(0, 3);
	// solution->swap(0, 4);

	// for (int i = 0; i < solution->getN(); i++)
	// 	cout << solution->getAssignment()[i] << " ";
	// cout << endl;
	
	// solution->computeCost();
	// cout << "cost = " << solution->getCost() << endl;

	// delta = solution->evalSwap(0, 8);
	// if(delta < 1) {
	// 	cout << "delta (" << 0 << ", " << 8 << ") = " << delta << endl;
	// 	solution->swap(0, 8);
	// 	solution->updateCost(delta);
	// }

	cout << "cost = " << solution->getCost() << endl;
	
	solution->computeCost();

	cout << "cost = " << solution->getCost() << endl;

	// solution->print();

	delete solution;
}

int main() {
	srand(1607);
	for(int i = 0; i < 10; i++)
		solve();
	return 0;
}