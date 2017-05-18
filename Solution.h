#ifndef Solution_h
#define Solution_h

#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>

using namespace std;

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

        // Get the (coord1, coord2, rotation) representation for the solution
        vector< vector <int> > mapAssignment();

        // Rotate piece $i to direction $r and update cost. 0: 0 degrees, 1: 90 degrees, 2: 180 degrees, 3: 270 degrees
        void rotate(int i, int r);

        // Apply swap move and update cost
		void swap(int i, int j);

		// Get the contribution to the cost given by piece at position $i
		int getCost(int i, int r);

		// Get the contribution to the cost given by piece at position $i if it moves to position $j
		int getCost(int i, int j, int r);

        // Compute the solution cost from scratch
        void computeCost();

        // Get the color pattern of tile $t in position $e with rotation $r
		int getColor(int i, int e, int r);

		// Evaluate the incurring cost if piece in position $i is rotated in direction $r 
		int evalRotate(int i, int r);

		// Evaluate the incurring cost if pieces in position $i and $j are swaped
		int evalSwap(int i, int j);

		// Update the total cost. Typicaly called after a move
		void updateCost(int delta);

		// Get the number of pieces in the puzzle
		int getN() { return n; };

		int top(int i);

		int bottom(int i);

		int left(int i);

		int rigth(int i);

		// Print the solution in format (coord1, coord2, rotation) for each piece
		void print();

		void printAssignment();
};

#endif