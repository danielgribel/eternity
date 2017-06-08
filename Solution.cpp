#include "Solution.h"

using namespace std;

Solution::Solution(vector< vector<int> > pieces, vector<int> assignment, vector<int> rotation, double cost) {
	this->pieces = pieces;
	this->n = pieces.size();
	this->assignment = assignment;
	this->position = vector<int> (n);
	this->rotation = rotation;
	this->pieceType = vector<int> (n);
	this->cost = cost;
	categorizePieces();
	computeCost();
}

Solution::Solution(vector< vector<int> > pieces,
					vector<int> assignment,
					vector<int> rotation,
					vector<int> position,
					vector<int> pieceType, 
					double cost) {
	
	this->pieces = pieces;
	this->assignment = assignment;
	this->rotation = rotation;
	this->position = position;
	this->pieceType = pieceType;
	this->cost = cost;
	this->n = pieces.size();
}

Solution::Solution(vector< vector<int> > pieces) {
	this->pieces = pieces;
	this->n = pieces.size();
	this->assignment = vector<int> (n);
	this->position = vector<int> (n);
	this->rotation = vector<int> (n);
	this->pieceType = vector<int> (n);
	cost = 0.0;
	categorizePieces();
	initialAssignment();
	computeCost();
}

Solution::~Solution() {

}

void Solution::initialAssignment() {
	int d = sqrt(n);
	int j = 0;
	Util u;

	// Assign corner tiles to positions in the corner of the puzzle
	vector<int> shCorners = u.shuffle(corners.size());
	assignment[0] = corners[shCorners[0]];
	assignment[d-1] = corners[shCorners[1]];
	assignment[d*(d-1)] = corners[shCorners[2]];
	assignment[d*d - 1] = corners[shCorners[3]];

	// Rotate to position the grey patterns in the border of the puzzle
	rotation[assignment[0]] = 3;
	rotation[assignment[d-1]] = 0;
	rotation[assignment[d*(d-1)]] = 2;
	rotation[assignment[d*d - 1]] = 1;

	// Assign border tiles to positions in the border of the puzzle
	vector<int> shBorders = u.shuffle(borders.size());
	for (int i = 1; i < d-1; i++) { // Top border
		assignment[i] = borders[shBorders[j]];
		rotation[assignment[i]] = 0;
		j++;
	}
	for (int i = d*(d-1) + 1; i < (d*d)-1; i++) { // Bottom border
		assignment[i] = borders[shBorders[j]];
		rotation[assignment[i]] = 2;
		j++;
	}
	for (int i = d; i < d*(d-1); i = i+d) { // Left border
		assignment[i] = borders[shBorders[j]];
		rotation[assignment[i]] = 3;
		j++;
	}
	for (int i = d+(d-1); i < (d*d)-1; i = i+d) { // Rigth border
		assignment[i] = borders[shBorders[j]];
		rotation[assignment[i]] = 1;
		j++;
	}

	// Assign inner tiles to positions in the inner area of the puzzle
	int s;
	int q = 0;
	vector<int> shInners = u.shuffle(inners.size());
	for (int y = 1; y < d-1; y++) {
		s = y*d + 1;
		for (int i = 0; i < d-2; i++) {
			assignment[s + i] = inners[shInners[q]];
			rotation[assignment[s + i]] = rand() % 4;
			q++;
		}
	}

	for(int i = 0; i < n; i++) {
		position[assignment[i]] = i;
	}
}

void Solution::categorizePieces() {
	int countZeros;
	for (int i = 0; i < n; i++) {
    	countZeros = 0;
        for (int j = 0; j < 4; j++) {
        	if(pieces[i][j] == 0) {
        		countZeros++;
        	}
        }
        if(countZeros == 2) {
        	corners.push_back(i);
        	pieceType[i] = 0;
        } else if(countZeros == 1) {
        	borders.push_back(i);
        	pieceType[i] = 1;
        } else {
        	inners.push_back(i);
        	pieceType[i] = 2;
        }
    }
}

void Solution::fixBorders() {
	int d = sqrt(n);
	for(int i = 1; i < d; i++) {
		rotate(i, 0);
	}
	for(int i = 0; i < d*(d-1); i=i+d) {
		rotate(i, 3);	
	}
	for(int i = d+d-1; i < n; i=i+d) {
		rotate(i, 1);
	}
	for(int i = d*(d-1); i < n-1; i++) {
		rotate(i, 2);
	}
	computeCost();
}

int Solution::getPieceType(int pos) {
	return pieceType[assignment[pos]];
}
        
vector< vector <int> > Solution::mapAssignment() {
	int d = sqrt(n);
	vector< vector <int> > m (n, vector<int>(3));
	vector<int> prtAssignment (assignment.size());
	for (int i = 0; i < assignment.size(); i++) {
		prtAssignment[assignment[i]] = i;
	}
	int r;
	for (int i = 0; i < n; i++) {
		r = rotation[i];
		if(pieceType[i] == 2) {
			if(rotation[i] == 0)
				r = 3;
			else if(rotation[i] == 1)
				r = 0;
			else if(rotation[i] == 2)
				r = 1;
			else if(rotation[i] == 3)
				r = 2;	
		}
		m[i][0] = prtAssignment[i]/d;
		m[i][1] = prtAssignment[i]%d;
		m[i][2] = r;
		// m[i][2] = rotation[i];
	}
	return m;
}

void Solution::computeCost() {
	int penalty = 0;
	for (int i = 0; i < n; i++) {
		penalty = penalty + getCost(i);
	}
	cost = penalty/2; // Dividing by 2 because we are counting twice
}

int Solution::getColor(int i, int e, int r) {
	int z = e - r;
	return pieces[assignment[i]][(z%4 + 4)%4];
}

int Solution::evalRotate(int i, int r) {
	int prevCost = getCost(i, rotation[assignment[i]]);
	int postCost = getCost(i, r);
	return postCost - prevCost; 
}

void Solution::rotate(int i, int r) {
	rotation[assignment[i]] = r;
}

void Solution::updateCost(int delta) {
	cost = cost + delta;
}

int Solution::evalSwap(int p1, int p2) {
	evalSwap(p1, p2, rotation[assignment[p1]], rotation[assignment[p2]]);
}

int Solution::evalSwap(int p1, int p2, int r1, int r2) {
	int d = sqrt(n);
	int i = p1;
	int j = p2;
	int rotI = r1;
	int rotJ = r2;
	
	if(p1 > p2) {
		i = p2;
		j = p1;
		rotI = r2;
		rotJ = r1;
	}

	int prevCostI = getCost(i, rotation[assignment[i]]);
	int prevCostJ = getCost(j, rotation[assignment[j]]);
	int postCostI = getCost(i, j, rotI, rotJ);
	int postCostJ = getCost(j, i, rotJ, rotI);
	int delta = postCostI + postCostJ - prevCostI - prevCostJ;

	if(j-i == d) {
		int bottomI = bottom(i);
		int bottomJ = bottom(j, rotJ);
		int topI = top(i, rotI);
		int topJ = top(j);

		if(bottomI != topJ && bottomJ == topI) {
			delta++;
		}
		if(bottomI == topJ && bottomJ != topI) {
			delta--;
		}
	}

	if(j-i == 1 && (j % d != 0)) {
		int rigthI = rigth(i);
		int rigthJ = rigth(j, rotJ);
		int leftI = left(i, rotI);
		int leftJ = left(j);

		if(rigthI != leftJ && rigthJ == leftI) {
			delta++;
		}
		if(rigthI == leftJ && rigthJ != leftI) {
			delta--;
		}
	}

	return delta;
}

void Solution::swap(int i, int j) {
	int tmp = assignment[i];
	assignment[i] = assignment[j];
	position[assignment[j]] = i;
	assignment[j] = tmp;
	position[tmp] = j;
}

int Solution::getCost(int i) {
	int r = rotation[assignment[i]];
	return getCost(i, i, r, r);
}

int Solution::getCost(int i, int r) {
	return getCost(i, i, r, r);
}

int Solution::getCost(int i, int j, int rotI, int rotJ) {
	const int d = sqrt(n);
	int penalty = 0;
	if((j-d) >= 0) {
		if((j-d) == i) {
			if(bottom(j, rotJ) != top(i, rotI)) {
				penalty++;
			}	
		} else {
			if(bottom(j-d) != top(i, rotI)) {
				penalty++;
			}	
		}
	}
	if( ((j-1) >= 0) && (j % d != 0) ) {
		if((j-1) == i) {
			if(rigth(j, rotJ) != left(i, rotI)) {
				penalty++;
			}	
		} else {
			if(rigth(j-1) != left(i, rotI)) {
				penalty++;
			}	
		}
	}
	if( ((j+1) < n) && ((j+1) % d != 0) ) {
		if((j+1) == i) {
			if(left(j, rotJ) != rigth(i, rotI)) {
				penalty++;
			}	
		} else {
			if(left(j+1) != rigth(i, rotI)) {
				penalty++;
			}	
		}
	}
	if((j+d) < n) {
		if((j+d) == i) {
			if(top(j, rotJ) != bottom(i, rotI)) {
				penalty++;
			}	
		} else {
			if(top(j+d) != bottom(i, rotI)) {
				penalty++;
			}
		}
	}
	return penalty;
}

int Solution::trySwap(int i, int j) {
	int bestDelta = MAX_INT;
	if(i != j) {
		int bestR1, bestR2;
		int delta;
		for (int r1 = 0; r1 < 4; r1++) {
			for (int r2 = 0; r2 < 4; r2++) {
				delta = evalSwap(i, j, r1, r2);
				if(delta < bestDelta) {
					bestR1 = r1;
					bestR2 = r2;
					bestDelta = delta;
				}
			}
		}
		if(bestDelta < 1) {
			rotate(i, bestR1);
			rotate(j, bestR2);
			swap(i, j);
			updateCost(bestDelta);
		}
	}
	return bestDelta;
}

MoveWrapper Solution::swapWrapper(int i, int j) {
	int bestR1, bestR2;
	int delta;
	int bestDelta = MAX_INT;
	for (int r1 = 0; r1 < 4; r1++) {
		for (int r2 = 0; r2 < 4; r2++) {
			delta = evalSwap(i, j, r1, r2);
			if(delta < bestDelta) {
				bestR1 = r1;
				bestR2 = r2;
				bestDelta = delta;
			}
		}
	}
	MoveWrapper mw;
	mw.pos1 = i;
	mw.pos2 = j;
	mw.rot1 = bestR1;
	mw.rot2 = bestR2;
	mw.deltaCost = bestDelta;
	return mw;
}

void Solution::rotateAndSwap(MoveWrapper mw) {
	rotate(mw.pos1, mw.rot1);
	rotate(mw.pos2, mw.rot2);
	swap(mw.pos1, mw.pos2);
	updateCost(mw.deltaCost);
}

void Solution::print() {
	vector< vector <int> > m = mapAssignment();
	for (int i = 0; i < m.size(); i++) {
		cout << m[i][0] << " " << m[i][1] << " " << m[i][2] << endl;
	}
}

void Solution::printAssignment() {
	for (int i = 0; i < n; i++) {
		cout << assignment[i] << " ";
	}
	cout << endl;
}

void Solution::printRotation() {
	for (int i = 0; i < n; i++) {
		cout << i << ": " << rotation[i] << endl;
	}
	// cout << endl;
}

int Solution::top(int i, int r) {
	return getColor(i, 0, r);
}

int Solution::rigth(int i, int r) {
	return getColor(i, 1, r);
}

int Solution::bottom(int i, int r) {
	return getColor(i, 2, r);
}

int Solution::left(int i, int r) {
	return getColor(i, 3, r);
}

int Solution::top(int i) {
	return getColor(i, 0, rotation[assignment[i]]);
}

int Solution::rigth(int i) {
	return getColor(i, 1, rotation[assignment[i]]);
}

int Solution::bottom(int i) {
	return getColor(i, 2, rotation[assignment[i]]);
}

int Solution::left(int i) {
	return getColor(i, 3, rotation[assignment[i]]);
}

LocalSolutionWrapper Solution::getBestRotation(vector<int> permut, vector<int> region, int r) {
	int err;
	int d = sqrt(n);
	int q = 0;
	int min = MAX_INT;
	vector<int> bestRot(4);
	LocalSolutionWrapper lsw;
	for(int r0 = 0; r0 < 4; r0++) {
		for(int r1 = 0; r1 < 4; r1++) {
			for(int r2 = 0; r2 < 4; r2++) {
				for(int r3 = 0; r3 < 4; r3++) {
					err = 0;
					if(rigth(permut[0], r0) != left(permut[1], r1)) {
						err++;
					}
					if(rigth(permut[2], r2) != left(permut[3], r3)) {
						err++;
					}
					if(bottom(permut[0], r0) != top(permut[2], r2)) {
						err++;
					}
					if(bottom(permut[1], r1) != top(permut[3], r3)) {
						err++;
					}
					if((region[0]-d >= 0) && (top(permut[0], r0) != bottom(region[0]-d))) {
						err++;
					}
					if((region[0]-1 >= 0) && (region[0] % d != 0) && (left(permut[0], r0) != rigth(region[0]-1))) {
						err++;
					}
					if((region[1]-d >= 0) && (top(permut[1], r1) != bottom(region[1]-d))) {
						err++;
					}
					if((region[1]+1 < n) && ((region[1]+1) % d != 0) && (rigth(permut[1], r1) != left(region[1]+1))) {
						err++;
					}
					if((region[2]+d < n) && (bottom(permut[2], r2) != top(region[2]+d))) {
						err++;
					}
					if((region[2]-1 >= 0) && (region[2] % d != 0) && (left(permut[2], r2) != rigth(region[2]-1))) {
						err++;
					}
					if((region[3]+d < n) && (bottom(permut[3], r3) != top(region[3]+d))) {
						err++;
					}
					if((region[3]+1 < n) && ((region[3]+1) % d != 0) && (rigth(permut[3], r3) != left(region[3]+1))) {
						err++;
					}

					if(err < min) {
						min = err;
						bestRot[0] = r0;
						bestRot[1] = r1;
						bestRot[2] = r2;
						bestRot[3] = r3;
						lsw.error = err;
						lsw.rotation = bestRot;
						lsw.permutation = permut;
						lsw.region = r;
						if(err == 0) {
							return lsw;
						}
					}
				}
			}
		}
	}

	return lsw;
}

LocalSolutionWrapper Solution::costAssign(int b, int p, vector< vector<int> > block) {
	int err;
	int d = sqrt(n);
	int q = 0;
	int min = MAX_INT;
	vector<int> bestRot(4);
	LocalSolutionWrapper lsw;
	for(int r0 = 0; r0 < 4; r0++) {
		for(int r1 = 0; r1 < 4; r1++) {
			for(int r2 = 0; r2 < 4; r2++) {
				for(int r3 = 0; r3 < 4; r3++) {
					err = 0;
					// The cost of inner edges of block $b
					if(rigth(block[b][0], r0) != left(block[b][1], r1)) {
						err++;
					}
					if(rigth(block[b][2], r2) != left(block[b][3], r3)) {
						err++;
					}
					if(bottom(block[b][0], r0) != top(block[b][2], r2)) {
						err++;
					}
					if(bottom(block[b][1], r1) != top(block[b][3], r3)) {
						err++;
					}
					// The cost of outer edges if block $b is assigned to position $p
					if((block[p][0]-d >= 0) && (top(block[b][0], r0) != bottom(block[p][0]-d))) {
						err++;
					}
					if((block[p][0]-1 >= 0) && (left(block[b][0], r0) != rigth(block[p][0]-1))) {
						err++;
					}
					if((block[p][1]-d >= 0) && (top(block[b][1], r1) != bottom(block[p][1]-d))) {
						err++;
					}
					if((block[p][1]+1 < n) && (rigth(block[b][1], r1) != left(block[p][1]+1))) {
						err++;
					}
					if((block[p][2]+d < n) && (bottom(block[b][2], r2) != top(block[p][2]+d))) {
						err++;
					}
					if((block[p][2]-1 >= 0) && (left(block[b][2], r2) != rigth(block[p][2]-1))) {
						err++;
					}
					if((block[p][3]+d < n) && (bottom(block[b][3], r3) != top(block[p][3]+d))) {
						err++;
					}
					if((block[p][3]+1 < n) && (rigth(block[b][3], r3) != left(block[p][3]+1))) {
						err++;
					}

					if(err < min) {
						min = err;
						bestRot[0] = r0;
						bestRot[1] = r1;
						bestRot[2] = r2;
						bestRot[3] = r3;
						lsw.error = err;
						lsw.rotation = bestRot;
						lsw.permutation = block[b];
						lsw.region = b;
						if(err == 0) {
							return lsw;
						}
					}
				}
			}
		}
	}

	return lsw;
}

void Solution::computeAssignment(vector<int> listOfBlocks, vector< vector<int> > blocks) {
	const int s = listOfBlocks.size();
	vector< vector<LocalSolutionWrapper> > G (s, vector<LocalSolutionWrapper>(s));
	vector< vector<double> > G_ (s, vector<double>(s));

	for(int i = 0; i < s; i++) {
		for(int j = 0; j < s; j++) {
			LocalSolutionWrapper X = costAssign(listOfBlocks[i], listOfBlocks[j], blocks);
			G[i][j] = X;
			G_[i][j] = X.error;
		}
	}

	VI Lmate;
	VI Rmate;
	Util x;
	double c = x.MinCostMatching(G_, Lmate, Rmate);
	vector< vector<int> > rot (s, vector<int>(4));

	// TO-DO: Rotate blocks
	for(int i = 0; i < s; i++) {
		rot[i] = G[i][Lmate[i]].rotation;
		for(int j = 0; j < 4; j++) {
			rotate(blocks[listOfBlocks[i]][j], rot[i][j]);
		}
	}

	vector<int> tmpAssignment = assignment;

	for(int i = 0; i < Lmate.size(); i++) {
		if(i != Lmate[i]) {
			placeBlock(listOfBlocks[i], listOfBlocks[Lmate[i]], tmpAssignment, blocks);
		}
	}
	
	computeCost();
}

void Solution::placeBlock(int b, int p, vector<int> tmpAssignment, vector< vector<int> > blocks) {
	assign(tmpAssignment[blocks[b][0]], blocks[p][0]);
	assign(tmpAssignment[blocks[b][1]], blocks[p][1]);
	assign(tmpAssignment[blocks[b][2]], blocks[p][2]);
	assign(tmpAssignment[blocks[b][3]], blocks[p][3]);
}

// int regionCost(vector<int> region) {
// 	int err = 0;
// 	for(int i = 0; i < region.size(); i++) {
// 		err = err + getCost(region[i], rotation[assignment[region[i]]]);
// 	}
// }

void Solution::assign(int piece, int pos) {
	assignment[pos] = piece;
	position[piece] = pos;
}

int Solution::getPosition(int piece) {
	return position[piece];
}