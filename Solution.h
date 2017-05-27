#ifndef Solution_h
#define Solution_h

#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>
#include <limits>
#include "MoveWrapper.h"
#include "LocalSolutionWrapper.h"

using namespace std;

const int MAX_INT = numeric_limits<int>::max();

class Solution {

	private:

		vector< vector<int> > pieces;
    	
    	vector<int> assignment;
    	
    	vector<int> rotation;
        
        double cost;

        vector<int> corners;
	    
	    vector<int> borders;
	    
	    vector<int> inners;

	    int n;

	    vector<int> pieceType;

	    void categorizePieces();

	    void initialAssignment();

    public:

    	Solution(vector< vector<int> > pieces, vector<int> assignment, vector<int> rotation, double cost);
        
        Solution(vector< vector<int> > pieces);
        
        ~Solution();
        
        vector< vector<int> > getPieces() { return pieces; };
    	
    	vector<int> getAssignment() { return assignment; };
    	
    	vector<int> getRotation() { return rotation; };

    	// Get the total cost
        double getCost() { return cost; };

        // Get the corner pieces (ids)
        vector<int> getCorners() { return corners; };

        // Get the border pieces (ids)
        vector<int> getBorders() { return borders; };

        // Get the inner pieces (ids)
        vector<int> getInners() { return inners; };

        void fixBorders();

        int getPieceType(int pos);

        // Get the (coord1, coord2, rotation) representation for the solution
        vector< vector <int> > mapAssignment();

        // Rotate piece $i to direction $r and update cost. 0: 0 degrees, 1: 90 degrees, 2: 180 degrees, 3: 270 degrees
        void rotate(int i, int r);

        // Apply swap move and update cost
		void swap(int i, int j);

		// Get the contribution to the cost given by piece at position $i in its current rotation
		int getCost(int i);

		// Get the contribution to the cost given by piece at position $i considering a rotation $r
		int getCost(int i, int r);

		// Get the contribution to the cost given by piece at position $i if it moves to position $j
		int getCost(int i, int j, int rotI, int rotJ);

        // Compute the solution cost from scratch
        void computeCost();

        // Get the color pattern of tile $t in position $e with rotation $r
		int getColor(int i, int e, int r);

		// Evaluate the incurring cost if piece in position $i is rotated in direction $r 
		int evalRotate(int i, int r);

		// Evaluate the incurring cost if pieces in position $i and $j are swaped
		int evalSwap(int p1, int p2, int r1, int r2);

		int evalSwap(int p1, int p2);

		// Update the total cost. Typicaly called after a move
		void updateCost(int delta);

		// Get the number of pieces in the puzzle
		int getN() { return n; };

		int trySwap(int i, int j);

		MoveWrapper swapWrapper(int i, int j);

		void rotateAndSwap(MoveWrapper mw);

		int top(int i, int r);

		int rigth(int i, int r);

		int bottom(int i, int r);

		int left(int i, int r);

		int top(int i);

		int rigth(int i);

		int bottom(int i);

		int left(int i);

		// Print the solution in format (coord1, coord2, rotation) for each piece
		void print();

		void printAssignment();

		void printRotation();

		LocalSolutionWrapper getBestRotation(vector<int> permut, vector<int> region, int r);

		void assign(int piece, int pos);
};

#endif