#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <string>  
#include <cmath>  
#include <math.h>
#include <vector>

using namespace std;

static inline void ProgressBar(unsigned int i, unsigned int max, unsigned int incr)
{
	int w = 100 / incr;
	double ratio = i / double(max);
	if (i % (max / w) == 0)
	{
		std::cout << "\r" << flush;
		std::cout << ceil(double(i) / double(max) * 100) << "%";
	}
	if (i == (max - 1)) std::cout << "" << endl;
}

bool PatchCheck(vector<int> patchList, int cell)
{
	for (int i = 0; i < patchList.size(); i++)
	{
		if (cell == patchList[i]) return true;
	}
	return false;
};

int RightLower(int cell, int L)
{
	if (int(floor(cell / L)) % 2 == 0) return cell + L;
	else if (int(floor(cell / L)) % 2 != 0)
	{
		if ((floor(cell / L)) == (L - 1)) return ((cell % L) + 1);
		else return cell + L + 1;
	}
};
int LeftLower(int cell, int L)
{
	if ((int(floor(double(cell) / double(L)))) % 2 == 0) return cell + L - 1;
	else if ((int(floor(double(cell) / double(L)))) % 2 != 0)
	{
		if ((floor(double(cell) / double(L))) == (L - 1)) return (cell % L);
		else return cell + L;
	}
};
int RightUpper(int cell, int L)
{
	if ((int(floor(double(cell) / double(L)))) % 2 == 0)
	{
		if (floor(cell / L) == 0) return ((L * L) - (L - (cell % L)));
		else return cell - L;
	}
	else if ((int(floor(double(cell) / double(L)))) % 2 != 0) return cell - L + 1;
}
int LeftUpper(int cell, int L)
{
	if ((int(floor(double(cell) / double(L)))) % 2 == 0)
	{
		if (floor(cell / L) == 0) return  ((L * L) - (L - (cell % L)) - 1);
		else return cell - L - 1;
	}
	else if ((int(floor(double(cell) / double(L)))) % 2 != 0) return cell - L;
};

bool PacemakerCheck(int cell, int L)
{
	if ((cell % L) == 0) return true;
	else return false;
};

bool LastColumnCheck(int cell, int L)
{
	if ((cell + 1) % L == 0) return true;
	else return false;
};

int FindRadius(vector<int> perimeterList, int starterCell, int L)
{
	int centralColumn = starterCell % L;
	int centralRow = floor(starterCell / L);
	for (int i = 0; i < perimeterList.size(); i++)
	{
		int cell = perimeterList[i];
		if (floor(cell / L) == centralRow) return abs(centralColumn - (cell % L));
		else if ((cell % L) == centralColumn) return abs(centralRow - floor(cell / L));
	}
	return 0;
};

bool PercolationCheck(vector<int> patchList, vector<int> perimeterList, int starterCell, int couples[][3], int L, double percPercentage)
{
	bool percolation = false;

	double radius = FindRadius(perimeterList, starterCell, L);
	int centralColumn = starterCell % L;
	int centralRow = floor(starterCell / L);
	double boundary = radius / (2 * sqrt(2));
	vector<int> activeCells, nextActiveCells, connectedCells;
	unsigned long long growthSeed = 4318656578904;
	init_genrand64(growthSeed);

	for (int i = 0; i < perimeterList.size(); i++) if (abs(centralRow - (perimeterList[i] / L)) <= boundary && (perimeterList[i] % L) <= centralColumn)  activeCells.push_back(perimeterList[i]);
	int perimeterCount = 0;
	int edgeCount = 0;
	int perimeterSize = perimeterList.size();
	int threshold = percPercentage*(perimeterSize / 2);
	bool exhausted = false;
	do
	{
		for (int j = 0; j < activeCells.size(); j++)
		{
			//couple check
			//check left horizontal neighbour, check left neighbour is in patch
			if (couples[activeCells[j] - 1][2] == 1 && PatchCheck(patchList, activeCells[j] - 1) == true && PatchCheck(nextActiveCells, activeCells[j] - 1) == false && PatchCheck(connectedCells, activeCells[j] - 1) == false)
			{
				nextActiveCells.push_back(activeCells[j] - 1);
				connectedCells.push_back(activeCells[j] - 1);
			}
			//check right horizontal neighbour (don't need to worry about boundary since simulation stops for perimeter on right
			if (couples[activeCells[j]][2] == 1 && PatchCheck(patchList, activeCells[j] + 1) == true && PatchCheck(nextActiveCells, activeCells[j] + 1) == false && PatchCheck(connectedCells, activeCells[j] + 1) == false)
			{
				nextActiveCells.push_back(activeCells[j] + 1);
				connectedCells.push_back(activeCells[j] + 1);
			}

			int lowerLeft = LeftLower(activeCells[j], L);
			int lowerRight = RightLower(activeCells[j], L);
			int upperLeft = LeftUpper(activeCells[j], L);
			int upperRight = RightUpper(activeCells[j], L);

			//bottom left neighbour
			if (couples[activeCells[j]][0] == 1 && PatchCheck(patchList, lowerLeft) == true && PatchCheck(nextActiveCells, lowerLeft) == false && PatchCheck(connectedCells, lowerLeft) == false)
			{
				nextActiveCells.push_back(lowerLeft);
				connectedCells.push_back(lowerLeft);
			}
			//bottom right
			if (couples[activeCells[j]][1] == 1 && PatchCheck(patchList, lowerRight) == true && PatchCheck(nextActiveCells, lowerRight) == false && PatchCheck(connectedCells, lowerRight) == false)
			{
				nextActiveCells.push_back(lowerRight);
				connectedCells.push_back(lowerRight);
			}
			//top left
			if (couples[upperLeft][1] == 1 && PatchCheck(patchList, upperLeft) == true && PatchCheck(nextActiveCells, upperLeft) == false && PatchCheck(connectedCells, upperLeft) == false)
			{
				nextActiveCells.push_back(upperLeft);
				connectedCells.push_back(upperLeft);
			}
			//top right
			if (couples[upperRight][0] == 1 && PatchCheck(patchList, upperRight) == true && PatchCheck(nextActiveCells, upperRight) == false && PatchCheck(connectedCells, upperRight) == false)
			{
				nextActiveCells.push_back(upperRight);
				connectedCells.push_back(upperRight);
			}

			if (PatchCheck(perimeterList, activeCells[j]) == true && (activeCells[j] % L) > centralColumn)/* return true;*/edgeCount++;
			if (edgeCount > threshold) return true;
			/*	if ((edgeCount / perimeterSize) > 0.9) return true;*/

		}
		activeCells = nextActiveCells;
		if (nextActiveCells.size() == 0) exhausted = true;
		nextActiveCells.clear();
	} while (exhausted == false);
	return false;
};

//******************VISUALISATION PARAMETERS**************************
double GetX(int i, int L, double widthO)
{
	int b = i%L;
	double radiushex = double(widthO / 2) / double(L);
	double apothem = radiushex*cos(3.141592654 / 6);
	double horizontal_shift = radiushex - apothem;

	//IF EVEN ROW (TESSELATION CONDITIONS)
	if ((int(floor(double(i) / double(L))) % 2) == 0) return b*((2 * radiushex) - (2 * horizontal_shift));
	//IF ODD ROW (TESSELATION CONDITIONS)
	else if ((int(floor(double(i) / double(L))) % 2) == 1) return radiushex - horizontal_shift + (b*((2 * radiushex) - (2 * horizontal_shift)));
};

double GetY(int i, int L, double widthO)
{
	int a = floor(double(i) / double(L));
	double radiushex = double(widthO / 2) / double(L);
	double apothem = radiushex*cos(3.141592654 / 6);
	double vertical_shift = radiushex / 2;
	return (a * 2 * radiushex) - (a*vertical_shift);
};

double HeatRed(int i, char gridType)
{
	if (gridType == 's' && i == 3) return 255;
	else if (gridType == 'h' && i > 3) return 255;
	else return 0;
};

double HeatGreen(int i, char gridType)
{
	if (gridType == 's' && i == 2) return 255;
	else if ( gridType == 'h' && i >= 2 && i <= 4) return 255;
	else return 0;
};

double HeatBlue(int i, char gridType)
{
	if (gridType == 's' && i == 1) return 255;
	else if (gridType == 'h' && i <= 2) return 255;
	else return 0;
};
//******************VISUALISATION PARAMETERS**************************