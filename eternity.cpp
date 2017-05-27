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

#define InputFile "../set1/pieces_set_1/pieces_10x10.txt"

vector< vector<int> > load(string dataFileName) {
	// const int numPatterns = 6;

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
    // vector< vector<int> > features (n * m, vector<int>(numPatterns));

    // for (int i = 0; i < n * m; i++) {
    // 	for (int j = 0; j < numPatterns; j++) {
    // 		features[i][j] = 0;
    // 	} 	
    // }

    // Read the data values directly into the vector of pieces
    for (int i = 0; i < n * m; i++) {
        input >> pieces[i][0];
        input >> pieces[i][1];
        input >> pieces[i][2];
        input >> pieces[i][3];
        // features[i][pieces[i][0]]++;
        // features[i][pieces[i][1]]++;
        // features[i][pieces[i][2]]++;
        // features[i][pieces[i][3]]++;
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

vector<int> shuffle(vector<int> vec, size_t n) {
    vector<int> myArray = shuffle(n);
    vector<int> vecShf(n);
    for(int i = 0; i < n; i++) {
    	vecShf[i] = vec[myArray[i]];
    }
    return vecShf;
}

void printMoveWrapper(vector<MoveWrapper> moveWrappers) {
	for (int i = 0; i < moveWrappers.size(); i++) {
		cout << moveWrappers[i].pos1 << " " << moveWrappers[i].pos2 << " "
			<< moveWrappers[i].rot1 << " " << moveWrappers[i].rot1 << " " << moveWrappers[i].deltaCost
			<< endl;
	}
}

MoveWrapper getBestMoveWrapper(vector<MoveWrapper> moveWrappers) {
	int bestCost = MAX_INT;
	MoveWrapper bestMw;
	for (int i = 0; i < moveWrappers.size(); i++) {
		if(moveWrappers[i].deltaCost < bestCost) {
			bestCost = moveWrappers[i].deltaCost;
			bestMw = moveWrappers[i];
		}
	}
	return bestMw;
}

// Partial local optimization
void partialLocalOpt(Solution * solution, vector <int> region) {
	vector<MoveWrapper> moveWrappers;
	for (int i = 0; i < region.size(); i++) {
		for (int j = i+1; j < region.size(); j++) {
			if(i != j) {
				MoveWrapper mw = solution->swapWrapper(region[i], region[j]);
				moveWrappers.push_back(mw);
			}
		}
	}
	MoveWrapper bestMw = getBestMoveWrapper(moveWrappers);
	if(bestMw.deltaCost < 0) {
		solution->rotateAndSwap(bestMw);
	}
}

void swap(vector<int>& region, int x, int y) {
    int temp = region[x];
    region[x]=region[y];
    region[y]=temp;
}

void print(vector<int> p) {
    for (int i = 0; i < p.size(); i++)
        cout << p[i] << " ";
    cout << endl;
}

vector<int> getPermutation(vector<int> region, int size){
	vector<int> p(size);
    for (int i=0;i<size;i++)
        p[i] = region[i];
    return p;
}

void permute(vector<int> region, vector< vector<int> >& permutations, int k) {
    int i;
    int size = region.size();

    if (k==0) {
        vector<int> p = getPermutation(region, size);
        permutations.push_back(p);
    }
    else {
        for (i=k-1;i>=0;i--) {
            swap(region,i,k-1);
            permute(region, permutations, k-1);
            swap(region,i,k-1);
        }
    }
}

void optimizeLocally(Solution * solution, vector< vector <int> > regions, int r, int nbPieces) {

	// solution->computeCost();
	// cout << "cost PREV =" << solution->getCost() << endl;

	int nbPermutations = 1;
	for(int i = 2; i <= nbPieces; i++)
		nbPermutations = nbPermutations * i;

	vector< vector<int> > permutations;
	permute(regions[r], permutations, nbPieces);	
	vector<int> shufflePermut = shuffle(nbPermutations);
	LocalSolutionWrapper bestX;
	int p;
	int ix = 0;
	int min = MAX_INT;

	bool search = true;
	while(search && ix < nbPermutations) {
		p = shufflePermut[ix];
		// p = ix;
		LocalSolutionWrapper X = solution->getBestRotation(permutations[p], regions[r], r);
		if(X.error <= min) {
			min = X.error;
			bestX = X;
			if(X.error == 0) {
				search = false;
			}
		}
		ix++;
	}

	// Update assignments and rotations
	vector<int> tmpAssignment = solution->getAssignment();
	for(int i = 0; i < nbPieces; i++) {
		solution->rotate(bestX.permutation[i], bestX.rotation[i]);
	}
	for(int i = 0; i < nbPieces; i++) {
		solution->assign(tmpAssignment[bestX.permutation[i]], regions[r][i]);
	}

	solution->computeCost();
	// cout << "cost POST =" << solution->getCost() << endl;
	// cout << "err " << r << " = " << X.error << endl;
}

void optimize(Solution * solution, vector< vector <int> > regions, vector<int> innerBlock, int size) {
	vector<int> shRegions = shuffle(innerBlock, innerBlock.size());
	int q;
	for(int q1 = 0; q1 < innerBlock.size(); q1++) {
		q = shRegions[q1];
		optimizeLocally(solution, regions, q, size*size);
	}
}

void solve() {
	vector< vector<int> > pieces = load(InputFile); // Load the vector of pieces
	Solution * solution = new Solution(pieces);
	
	const int blockSize = 2;
	const int n = solution->getN();
	const int d = sqrt(n);
	const int nbEdges = 2*d*(d-1);
	const int nbRegions = (d/blockSize)*(d/blockSize);

	vector<int> shufflePiecesI;
	vector<int> shufflePiecesJ;
	int i, j, q, delta;
	
	vector< vector <int> > regions (nbRegions, vector<int>(blockSize * blockSize));
	int w = 0;
	for (int i = 0; i < nbRegions; i++) {
		for (int j = 0; j < blockSize; j++) {
			regions[i][j] = w + i * blockSize + j;
			for(int y = 1; y < blockSize; y++) {
				regions[i][y * blockSize + j] = regions[i][j] + y*blockSize*sqrt(nbRegions);
			}
		}
		if((i+1) % (int)sqrt(nbRegions) == 0) {
			w = w + d*(blockSize-1);
		}
	}

	vector<int> innerBlocks;
	vector<int> innerBlocks1;
	vector<int> innerBlocks2;
	int h = sqrt(nbRegions);
	int c = 0;

	// NOT WORKING FOR d = 8 and d = 16!!
	for(int i = 0; i < nbRegions; i++) {
		if( (i > h) && (i < h*h-h) && (i%h != 0) && ((i+1)%h != 0) ) {
			innerBlocks.push_back(i);
			c++;
			if(c % 2 == 0) {
				innerBlocks1.push_back(i);
			} else {
				innerBlocks2.push_back(i);
			}
		}
	}

	vector<int> pieceBlock (n);

	for (int i = 0; i < nbRegions; i++) {
		for (int j = 0; j < blockSize*blockSize; j++) {
			pieceBlock[regions[i][j]] = i;
		}
	}
	
	solution->computeCost();

	int it = 0;
	int imp, bestSwap, bestJ;
	while(it < 10) {
		
		shufflePiecesI = shuffle(n);
		for (int i1 = 0; i1 < n; i1++) {
			i = shufflePiecesI[i1];
			shufflePiecesJ = shuffle(n);
			MoveWrapper bestMw;
			bestMw.pos1 = i;
			bestMw.pos2 = i;
			bestMw.rot1 = 0;
			bestMw.rot2 = 0;
			bestMw.deltaCost = MAX_INT;
			for (int j1 = 0; j1 < n; j1++) {
				j = shufflePiecesJ[j1];
				if(solution->getPieceType(i) == 2 && solution->getPieceType(j) == 2) {
					if(i != j) {
						MoveWrapper mw = solution->swapWrapper(i, j);
						if(mw.deltaCost < bestMw.deltaCost) {
							bestMw = mw;
						}
					}
				}
			}
			if(bestMw.deltaCost < 1) {
				solution->rotateAndSwap(bestMw);
			}
		}
		if(it % 2 == 0) {
			optimize(solution, regions, innerBlocks1, blockSize);
			optimize(solution, regions, innerBlocks2, blockSize);
		} else {
			optimize(solution, regions, innerBlocks2, blockSize);
			optimize(solution, regions, innerBlocks1, blockSize);
		}
		// solution->fixBorders();
		// solution->computeCost();
		// cout << "cost = " << nbEdges - solution->getCost() << endl;
		it++;
	}
	// solution->computeCost();
	cout << nbEdges - solution->getCost() << "/" << nbEdges << endl;
	// cout << "-----------------------------------------------" << endl;
	delete solution;
}

int main() {
	srand(1607);
	for(int i = 0; i < 10; i++)
		solve();
	return 0;
}