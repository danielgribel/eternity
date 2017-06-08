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
    region[x] = region[y];
    region[y] = temp;
}

vector<int> getPermutation(vector<int> region, int size){
	vector<int> p(size);
    for (int i = 0; i < size; i++)
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

void optimizeLocallyCorner(Solution * solution, vector< vector <int> > regions, int r, int b) {
	Util u;
	const int nbPermutations = 2;
	const int nbPieces = b*b;
	const int d = sqrt(solution->getN());
	vector< vector<int> > permutations (nbPermutations, vector<int>(nbPieces));
	
	permutations[0][0] = regions[r][0];
	permutations[0][1] = regions[r][1];
	permutations[0][2] = regions[r][2];
	permutations[0][3] = regions[r][3];

	if(r == 0 || r == ((d/b)*(d/b)-1)) { // Top left or Bottom rigth corner
		permutations[1][0] = regions[r][0];
		permutations[1][1] = regions[r][2];
		permutations[1][2] = regions[r][1];
		permutations[1][3] = regions[r][3];	

	} else { // Top rigth or Bottom left corner
		permutations[1][0] = regions[r][3];
		permutations[1][1] = regions[r][1];
		permutations[1][2] = regions[r][2];
		permutations[1][3] = regions[r][0];	
	}

	vector<int> shufflePermut = u.shuffle(nbPermutations);
	LocalSolutionWrapper bestX;
	int p;
	int ix = 0;
	int min = MAX_INT;

	bool search = true;
	while(search && ix < nbPermutations) {
		p = shufflePermut[ix];
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
}

void optimizeLocallyBorder(Solution * solution, vector< vector <int> > regions, int r, int b) {
	Util u;
	const int nbPermutations = 4;
	const int nbPieces = b*b;
	const int d = sqrt(solution->getN());
	const int g = d/b;
	vector< vector<int> > permutations (nbPermutations, vector<int>(nbPieces));
	
	permutations[0][0] = regions[r][0];
	permutations[0][1] = regions[r][1];
	permutations[0][2] = regions[r][2];
	permutations[0][3] = regions[r][3];

	if(r < g || r > g*(g-1)) { // Apply rule for top/bottom border
		permutations[1][0] = regions[r][1];
		permutations[1][1] = regions[r][0];
		permutations[1][2] = regions[r][2];
		permutations[1][3] = regions[r][3];	

		permutations[2][0] = regions[r][0];
		permutations[2][1] = regions[r][1];
		permutations[2][2] = regions[r][3];
		permutations[2][3] = regions[r][2];	
		
		permutations[3][0] = regions[r][1];
		permutations[3][1] = regions[r][0];
		permutations[3][2] = regions[r][3];
		permutations[3][3] = regions[r][2];	

	} else { // Apply rule for left/rigth border
		permutations[1][0] = regions[r][0];
		permutations[1][1] = regions[r][3];
		permutations[1][2] = regions[r][2];
		permutations[1][3] = regions[r][1];	

		permutations[2][0] = regions[r][2];
		permutations[2][1] = regions[r][1];
		permutations[2][2] = regions[r][0];
		permutations[2][3] = regions[r][3];	
		
		permutations[3][0] = regions[r][2];
		permutations[3][1] = regions[r][3];
		permutations[3][2] = regions[r][0];
		permutations[3][3] = regions[r][1];
	}

	vector<int> shufflePermut = u.shuffle(nbPermutations);
	LocalSolutionWrapper bestX;
	int p;
	int ix = 0;
	int min = MAX_INT;

	bool search = true;
	while(search && ix < nbPermutations) {
		p = shufflePermut[ix];
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
}

void optimizeLocallyInner(Solution * solution, vector< vector <int> > regions, vector< vector<int> >& permutations, int r, int b) {

	// solution->computeCost();
	// cout << "cost PREV =" << solution->getCost() << endl;

	Util u;

	int nbPermutations = 1;
	const int nbPieces = b*b;
	for(int i = 2; i <= nbPieces; i++)
		nbPermutations = nbPermutations * i;

	// vector< vector<int> > permutations;
	// permute(regions[r], permutations, nbPieces);	
	vector<int> shufflePermut = u.shuffle(nbPermutations);
	LocalSolutionWrapper bestX;
	int p;
	int ix = 0;
	int min = MAX_INT;

	bool search = true;
	while(search && ix < nbPermutations) {
		p = shufflePermut[ix];
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

void optimizeCorner(Solution * solution, vector< vector <int> > regions, vector<int> blocks, int b) {
	Util u;
	vector<int> shRegions = u.shuffle(blocks);
	int q;
	for(int q1 = 0; q1 < blocks.size(); q1++) {
		q = shRegions[q1];
		optimizeLocallyCorner(solution, regions, q, b);
	}
}

void optimizeBorder(Solution * solution, vector< vector <int> > regions, vector<int> blocks, int b) {
	Util u;
	vector<int> shRegions = u.shuffle(blocks);
	int q;
	for(int q1 = 0; q1 < blocks.size(); q1++) {
		q = shRegions[q1];
		optimizeLocallyBorder(solution, regions, q, b);
	}
}

void optimizeInner(Solution * solution, vector< vector <int> > regions, vector< vector< vector<int> > >& permutations, vector<int> blocks, int b) {
	Util u;
	vector<int> shRegions = u.shuffle(blocks);
	int q;
	for(int q1 = 0; q1 < blocks.size(); q1++) {
		q = shRegions[q1];
		optimizeLocallyInner(solution, regions, permutations[q], q, b); // Optimize a inner block 
	}
}

void optimizeCorner(Solution * solution, vector< vector <int> > regions, vector<int> blocks, int b, int q) {
	optimizeLocallyCorner(solution, regions, q, b);
}

void optimizeBorder(Solution * solution, vector< vector <int> > regions, vector<int> blocks, int b, int q) {
	optimizeLocallyBorder(solution, regions, q, b);
}

void optimizeInner(Solution * solution, vector< vector <int> > regions, vector< vector< vector<int> > >& permutations, vector<int> blocks, int b, int q) {
	optimizeLocallyInner(solution, regions, permutations[q], q, b); // Optimize a inner block 
}

void solve() {
	clock_t begin = clock();
	vector< vector<int> > pieces = load(InputFile); // Load the vector of pieces
	Util u;
	Solution * solution = new Solution(pieces);	

	const int b = 2;
	const int n = solution->getN();
	const int d = sqrt(n);
	const int nbEdges = 2*d*(d-1);
	const int nbBlocks = (d/b)*(d/b);

	vector<int> shufflePiecesI;
	vector<int> shufflePiecesJ;
	int i, j, q, delta;
		
	vector< vector <int> > regions (nbBlocks, vector<int>(b * b));
	int w = 0;
	for (int i = 0; i < nbBlocks; i++) {
		for (int j = 0; j < b; j++) {
			regions[i][j] = w + i * b + j;
			for(int y = 1; y < b; y++) {
				regions[i][y * b + j] = regions[i][j] + y*b*sqrt(nbBlocks);
			}
		}
		if((i+1) % (int)sqrt(nbBlocks) == 0) {
			w = w + d*(b-1);
		}
	}

	vector<int> blockType (nbBlocks);
	vector<int> cornerBlocks;
	vector<int> borderBlocks;
	vector<int> innerBlocks;
	vector<int> innerBlocks1;
	vector<int> innerBlocks2;

	const int h = sqrt(nbBlocks);
	const int g = d/b;
	int c = 0;

	for(int r = 0; r < nbBlocks; r++) {
		if(r == 0 || r == g-1 || r == g*(g-1) || r == g*g-1) { // Corner blocks
			blockType[r] = 0;
			cornerBlocks.push_back(r);
		} else if(r < g || r > g*(g-1) || r % g == 0 || (r+1) % g == 0) { // Border blocks
			blockType[r] = 1;
			borderBlocks.push_back(r);
		} else { // Inner blocks
			blockType[r] = 2;
			innerBlocks.push_back(r);
		}
	}
	int t;
	for(int i = 1; i < g-1; i++) {
		t = 0;
		for(int j = i*g+1; t < g-2; j++) {
			if( (i+t) % 2 == 0 ) {
				innerBlocks1.push_back(j);
			} else {
				innerBlocks2.push_back(j);
			}
			t++;
		}
	}

	vector<int> pieceBlock (n);

	for (int i = 0; i < nbBlocks; i++) {
		for (int j = 0; j < b*b; j++) {
			pieceBlock[regions[i][j]] = i;
		}
	}

	solution->computeCost();

	vector < vector< vector<int> > > permutations (nbBlocks);
	
	for(int i = 0; i < nbBlocks; i++) {
		permute(regions[i], permutations[i], b*b);	
	}

	int it = 0;
	int imp, bestSwap, bestJ;

	vector<int> bestAssignment = solution->getAssignment();
	vector<int> bestRotation = solution->getRotation();
	int bestCost = solution->getCost(); 

	while(it < 50000) {

		int rnd = rand() % 2;

		if(rnd == 0) {
			shufflePiecesI = u.shuffle(n);
			for (int i1 = 0; i1 < n; i1++) {
				i = shufflePiecesI[i1];
				shufflePiecesJ = u.shuffle(n);
				for (int j1 = 0; j1 < n; j1++) {
					j = shufflePiecesJ[j1];
					if(i != j && solution->getPieceType(i) == solution->getPieceType(j)) {
						MoveWrapper mw = solution->swapWrapper(i, j);
						if(mw.deltaCost < 1) {
							solution->rotateAndSwap(mw);
							if(solution->getPieceType(i) == 0)
								optimizeCorner(solution, regions, cornerBlocks, b);
							if(solution->getPieceType(i) == 1)
								optimizeBorder(solution, regions, borderBlocks, b);
							if(solution->getPieceType(i) == 2 && innerBlocks.size() > 0)
								optimizeInner(solution, regions, permutations, innerBlocks, b);
						}
					}
				}
			}
		}

		if(rnd == 1) {
			if(innerBlocks.size() > 1) {
				vector<int> s1 (innerBlocks1.size());
				vector<int> s2 (innerBlocks2.size());
				int subSetSize;
				if(s1.size() > s2.size()) {
					subSetSize = s1.size()/2 + 1;	
				} else {
					subSetSize = s2.size()/2 + 1;
				}
				
				vector<int> sub1 (subSetSize);
				vector<int> sub2 (subSetSize);
				bool search = true;
				int iter = 0;
				int prevCost = solution->getCost();
				while(search) {
					s1 = u.shuffle(innerBlocks1);
					s2 = u.shuffle(innerBlocks2);
					for(int j = 0; j < subSetSize; j++) {
						sub1[j] = s1[j];
						sub2[j] = s2[j];
					}
					if(iter % 2 == 0) {
						solution->computeAssignment(sub1, regions);
						solution->computeAssignment(sub2, regions);
					} else {
						solution->computeAssignment(sub2, regions);
						solution->computeAssignment(sub1, regions);
					}
					if(solution->getCost() < prevCost) {
						prevCost = solution->getCost();
					} else {
						search = false;			
					}
					iter++;
					optimizeCorner(solution, regions, cornerBlocks, b);
					optimizeBorder(solution, regions, borderBlocks, b);
					if(innerBlocks.size() > 0) {
						optimizeInner(solution, regions, permutations, innerBlocks, b);
					}
				}
			}
		}
		
		solution->fixBorders();
		it++;

		if(solution->getCost() < bestCost) {
			bestCost = solution->getCost();
			bestRotation = solution->getRotation();
			bestAssignment = solution->getAssignment();
		}
		// cout << "  ------- " << nbEdges - bestCost << endl;
	}
	
	Solution * bestSolution = new Solution(solution->getPieces(), bestAssignment, bestRotation, solution->getPosition(), solution->getPieceType(), bestCost);
	// bestSolution->print();

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	// cout << nbEdges - solution->getCost() << " " << nbEdges << " " << elapsed_secs << endl;
	cout << nbEdges - bestSolution->getCost() << " " << nbEdges << " " << elapsed_secs << endl;
	
	delete solution;
	delete bestSolution;
}

int main(int argc, char** argv) {
	int seed = atoi(argv[1]);
	srand(seed);
	solve();
	return 0;
}