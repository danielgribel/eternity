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

        double getCost() { return cost; };

        vector<int> getCorners() { return corners; };

        vector<int> getBorders() { return borders; };

        vector<int> getInners() { return inners; };

        // Get the (coord1, coord2, rotation) representation for the solution
        vector< vector <int> > mapAssignment();

        // Rotate piece $p to direction $r and update cost. 0: 0 degrees, 1: 90 degrees, 2: 180 degrees, 3: 270 degrees
        void rotate(int p, int r);

        // Apply swap move and update cost
		void swap(int i, int j);

		// Get the contribution to the cost given by piece at position $i
		int getCost(int p);

        // Compute the solution cost from scratch
        void computeCost();

        // Get the color pattern of tile $t in position $e, considering the current rotation $rotation
		int getColor(int t, int e);

};

#endif