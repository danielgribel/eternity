#include "Solution.h"

using namespace std;

Solution::Solution(vector< vector<int> > pieces, vector<int> assignment, vector<int> rotation, double cost) {
	this->pieces = pieces;
	this->n = pieces.size();
	this->assignment = assignment;
	this->rotation = rotation;
	this->cost = cost;
	categorizePieces();
	computeCost();
}

Solution::Solution(vector< vector<int> > pieces) {
	this->pieces = pieces;
	this->n = pieces.size();
	this->assignment = vector<int> (n);
	this->rotation = vector<int> (n);
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

	// Assign corner tiles to positions in the corner of the puzzle
	assignment[0] = corners[0];
	assignment[d-1] = corners[1];
	assignment[d*(d-1)] = corners[2];
	assignment[d*d - 1] = corners[3];

	// Rotate to position the grey patterns in the border of the puzzle
	rotation[assignment[0]] = 3;
	rotation[assignment[d-1]] = 0;
	rotation[assignment[d*(d-1)]] = 2;
	rotation[assignment[d*d - 1]] = 1;

	// Assign border tiles to positions in the border of the puzzle
	for (int i = 1; i < d-1; i++) { // Top border
		assignment[i] = borders[j];
		rotation[assignment[i]] = 0;
		j++;
	}
	for (int i = d*(d-1) + 1; i < (d*d)-1; i++) { // Bottom border
		assignment[i] = borders[j];
		rotation[assignment[i]] = 2;
		j++;
	}
	for (int i = d; i < d*(d-1); i = i+d) { // Left border
		assignment[i] = borders[j];
		rotation[assignment[i]] = 3;
		j++;
	}
	for (int i = d+(d-1); i < (d*d)-1; i = i+d) { // Rigth border
		assignment[i] = borders[j];
		rotation[assignment[i]] = 1;
		j++;
	}

	// Assign inner tiles to positions in the inner area of the puzzle
	int s;
	int q = 0;
	for (int y = 1; y < d-1; y++) {
		s = y*d + 1;
		for (int i = 0; i < d-2; i++) {
			assignment[s + i] = inners[q];
			q++;
		}
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
        } else if(countZeros == 1) {
        	borders.push_back(i);
        } else {
        	inners.push_back(i);
        }
    }
}
        
vector< vector <int> > Solution::mapAssignment() {
	int n = assignment.size();
	int d = sqrt(n);
	vector< vector <int> > m (n, vector<int>(3));
	vector<int> prtAssignment (assignment.size());
	for (int i = 0; i < assignment.size(); i++) {
		prtAssignment[assignment[i]] = i;
	}
	for (int i = 0; i < n; i++) {
		m[i][0] = prtAssignment[i]/d;
		m[i][1] = prtAssignment[i]%d;
		m[i][2] = rotation[i];
	}
	return m;
}

void Solution::computeCost() {
	int penalty = 0;
	for (int i = 0; i < n; i++) {
		penalty = penalty + getCost(i, rotation[assignment[i]]);
		// cout << getCost(i, rotation[assignment[i]]) << " ";
	}
	// cout << endl;
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

int Solution::evalSwap(int i, int j) {
	int prevCostI = getCost(i, rotation[assignment[i]]);
	int prevCostJ = getCost(j, rotation[assignment[j]]);
	int postCostI = getCost(i, j, rotation[assignment[i]]);
	int postCostJ = getCost(j, i, rotation[assignment[j]]);
	int delta = postCostI + postCostJ - prevCostI - prevCostJ;

	int d = sqrt(n);
	int min = i;
	int max = j;
	
	if(i > j) {
		min = j;
		max = i;
	}

	if(abs(i-j) == 4) {
		int bottomMin = getColor(min, 2, rotation[assignment[min]]);
		int bottomMax = getColor(max, 2, rotation[assignment[max]]);
		int topMin = getColor(min, 0, rotation[assignment[min]]);
		int topMax = getColor(max, 0, rotation[assignment[max]]);
		if(bottomMin != topMax && bottomMax == topMin) {
			delta++;
		}
		if(bottomMin == topMax && bottomMax != topMin) {
			delta--;
		}
	}

	if(abs(i-j) == 1 && (max % d != 0)) {
		int rigthMin = getColor(min, 1, rotation[assignment[min]]);
		int rigthMax = getColor(max, 1, rotation[assignment[max]]);
		int leftMin = getColor(min, 3, rotation[assignment[min]]);
		int leftMax = getColor(max, 3, rotation[assignment[max]]);
		if(rigthMin != leftMax && rigthMax == leftMin) {
			delta++;
		}
		if(rigthMin == leftMax && rigthMax != leftMin) {
			delta--;
		}
	}

	return delta;
}

void Solution::swap(int i, int j) {
	int tmp = assignment[i];
	assignment[i] = assignment[j];
	assignment[j] = tmp;
}

int Solution::getCost(int i, int r) {
	return getCost(i, i, r);
}

int Solution::getCost(int i, int j, int r) {
	const int d = sqrt(n);
	int penalty = 0;
	if((j-4) >= 0) {
		if((j-4) == i) {
			if(getColor(j, 2, rotation[assignment[j]]) != getColor(i, 0, r)) {
				penalty++;
			}	
		} else {
			if(getColor(j-4, 2, rotation[assignment[j-4]]) != getColor(i, 0, r)) {
				penalty++;
			}	
		}
	}
	if( ((j-1) >= 0) && (j % d != 0) ) {
		if((j-1) == i) {
			if(getColor(j, 1, rotation[assignment[j]]) != getColor(i, 3, r)) {
				penalty++;
			}	
		} else {
			if(getColor(j-1, 1, rotation[assignment[j-1]]) != getColor(i, 3, r)) {
				penalty++;
			}	
		}
	}
	if( ((j+1) < n) && ((j+1) % d != 0) ) {
		if((j+1) == i) {
			if(getColor(j, 3, rotation[assignment[j]]) != getColor(i, 1, r)) {
				penalty++;
			}	
		} else {
			if(getColor(j+1, 3, rotation[assignment[j+1]]) != getColor(i, 1, r)) {
				penalty++;
			}	
		}
	}
	if((j+4) < n) {
		if((j+4) == i) {
			if(getColor(j, 0, rotation[assignment[j]]) != getColor(i, 2, r)) {
				penalty++;
			}	
		} else {
			if(getColor(j+4, 0, rotation[assignment[j+4]]) != getColor(i, 2, r)) {
				penalty++;
			}
		}
	}
	return penalty;
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

// Given a position at $i, return the pattern in its top
int Solution::top(int i) {
	if( (i-4) < 0 ) {
		return -1;
	}
	return getColor(i-4, 2, rotation[assignment[i-4]]);
}

int Solution::bottom(int i) {
	if( (i+4) > n-1 ) {
		return -1;
	}
	return getColor(i+4, 0, rotation[assignment[i+4]]);
}

int Solution::left(int i) {
	if( (i-1) < 0 ) {
		return -1;
	}
	return getColor(i-1, 3, rotation[assignment[i-1]]);
}

int Solution::rigth(int i) {
	if( (i+1) > n-1 ) {
		return -1;
	}
	return getColor(i+1, 1, rotation[assignment[i+1]]);
}