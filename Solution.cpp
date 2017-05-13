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

	// Assign border tiles to positions in the border of the puzzle
	for (int i = 1; i < d-1; i++) { // Top border
		assignment[i] = borders[j];
		j++;
	}
	for (int i = d*(d-1) + 1; i < (d*d)-1; i++) { // Bottom border
		assignment[i] = borders[j];
		j++;
	}
	for (int i = d; i < d*(d-1); i = i+d) { // Left border
		assignment[i] = borders[j];
		j++;
	}
	for (int i = d+(d-1); i < (d*d)-1; i = i+d) { // Rigth border
		assignment[i] = borders[j];
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
	const int d = sqrt(n);
	int penalty = 0;
	for (int i = 0; i < n; i++) {
		penalty = penalty + getCost(i);
	}
	cost = penalty/2;
}

int Solution::getColor(int t, int e) {
	int z = e - rotation[t];
	return pieces[assignment[t]][(z%4 + 4)%4];
}

void Solution::rotate(int p, int r) {
	int prevCost = getCost(p);
	rotation[p] = r;
	int postCost = getCost(p);
	cost = cost + postCost - prevCost; 
}

void Solution::swap(int i, int j) {
	int prevCostI = getCost(i);
	int prevCostJ = getCost(j);
	int tmp = assignment[i];
	assignment[i] = assignment[j];
	assignment[j] = tmp;
	int postCostI = getCost(i);
	int postCostJ = getCost(j);
	cost = cost + postCostI + postCostJ - prevCostI - prevCostJ;
}

int Solution::getCost(int i) {
	const int d = sqrt(n);
	int penalty = 0;
	if((i-4) > 0) {
		if(getColor(i-4, 2) != getColor(i, 0)) {
			penalty++;
		}
	}
	if( ((i-1) > 0) && (i % d != 0) ) {
		if(getColor(i-1, 1) != getColor(i, 3)) {
			penalty++;
		}	
	}
	if( ((i+1) < n) && ((i+1) % d != 0) ) {
		if(getColor(i+1, 3) != getColor(i, 1)) {
			penalty++;
		}
	}
	if((i+4) < n) {
		if(getColor(i+4, 0) != getColor(i, 2)) {
			penalty++;
		}
	}
	return penalty;
}