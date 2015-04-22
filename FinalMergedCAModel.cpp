#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <string>  
#include <cmath>  
#include <math.h>
#include <stdio.h> 
#include <vector>
#include <ctime>
#include <iomanip>
#include "mt64.h"
#include "HelperFunctions.h"
#include <SFML/Graphics.hpp> 
using namespace std;

#define L 200 //lattice size
#define refp 50 //refractory period
#define pi 3.14159265 //convert radians to degrees

void main()
{
	char tmpbuf[128];
	_tzset();

	int tmax = 1000000;
	int SAP = 220; //period of pacemaker cells

	//******************INPUT VARIABLES***************************
	char gridtype;
	std::cout << "Hexagonal (h) or square (s) lattice?" << endl;
	cin >> gridtype;

	/*double globalNu;
	double nuHigh;
	{
		std::cout << "" << endl;
		std::cout << "Enter Nu " << endl;
		cin >> globalNu;
		std::cout << "" << endl;
		std::cout << "Enter Nu high " << endl;
		cin >> nuHigh;
	}*/

	int seedGroup;
	std::cout << "" << endl;
	std::cout << "Which seed group would you like to use? 1-10?" << endl;
	cin >> seedGroup;

	char CV;
	std::cout << "" << endl;
	std::cout << "Do you want to vary conduction velocity? (y/n)" << endl;
	cin >> CV;

	char exaggerate;
	if (CV == 'y')
	{
		std::cout << "" << endl;
		std::cout << "Do you also want to exaggerate the variation? (y/n)" << endl;
		cin >> exaggerate;
		tmax = 100000;
	}

	int factor;	//factor to exaggerate timedelay by
	if (exaggerate == 'y')
	{
		std::cout << "" << endl;
		std::cout << "By what factor do you want to exaggerate the variation? Insert number" << endl;
		cin >> factor;
	}

	char visualisation;
	std::cout << "" << endl;
	std::cout << "Do you want to enable visualisation? (y/n)" << endl;
	cin >> visualisation;

	char heatmap;
	std::cout << "" << endl;
	std::cout << "Do you want to enable heatmap? (y/n)" << endl;
	cin >> heatmap;

	char screenshot;
	if (visualisation == 'y' || heatmap == 'y')
	{
		std::cout << "" << endl;
		std::cout << "Do you want to take a screenshot? (y/n)" << endl;
		cin >> screenshot;
	}

	int timeStart;
	if (screenshot == 'y' || visualisation == 'y')
	{
		std::cout << "" << endl;
		std::cout << "At what time would you like to start visualisation/screen capture?" << endl;
		cin >> timeStart;
	}
	int c;

	char grow;
	std::cout << "" << endl;
	std::cout << "Patch? (y/n)" << endl;
	cin >> grow;

	int radius;
	if (grow == 'y')
	{
		std::cout << "" << endl;
		std::cout << "Radius?" << endl;
		cin >> radius;
	}
	else radius = 0;

	char patchType;
	if (grow == 'y')
	{
		std::cout << "" << endl;
		std::cout << "Regular(r) or Percolation Threshold(p)? " << endl;
		cin >> patchType;
	}

	

	//**************INITIALISE NU VARIABLES************
	double globalNu = 0.2;
	double localNu = 0.05, nu;
	double delta = 0.05;        //prob of defective cells
	double epsilon = 0.05;      //prob of cell not exciting
	double fib_thresh = L*1.2;  //threshold of number of excited cells to define 'in AF' state
	if (patchType == 'p') fib_thresh +=  0.2*L*radius / 70;
	double averagetimedelay = 0.0;
	int ecount = 0, AFtimecount = 0;
	int lowerLeft, lowerRight, upperLeft, upperRight;

	//***********VISUALISATION STUFF***************
	double widthO = 700;
	double radiushex = double(widthO / 2) / double(L);
	double sides = 6;
	double sizesquare = double(widthO) / double(L);
	double apothem = radiushex*cos(pi / (sides));
	double horizontal_shift = radiushex - apothem;
	double vertical_shift = radiushex / 2;
	double greyshadeinc = floor(255 / refp), redinc = floor(0 / refp), greeninc = floor(191 / refp), blueinc = floor(255 / refp);
	int width, height, a, b;

	//***********************PATCH VARIABLES**********************
	int patchGrowthTime = radius, patchCol = 100;
	double patchGrowthProb = 1.0, probabilityIncrement = 0.05, percPercentage = 0.3;
	int starterCell = ((L / 2)*L) + patchCol;	//always in middle row
	vector<int> patchList, activeCellList, nextActiveCellList, perimeterList;	//list of 'growable' cells

	//**************DRAW HEXAGONS**************
	sf::CircleShape cellexciteH(radiushex, sides);
	cellexciteH.setFillColor(sf::Color(50, 205, 50));
	sf::CircleShape cellrefractH(radiushex, sides);
	sf::CircleShape celldefectH(radiushex, sides);
	celldefectH.setFillColor(sf::Color(255, 0, 0));

	//**************DRAW SQUARES**************
	sf::RectangleShape cellexciteS(sf::Vector2f(sizesquare, sizesquare));
	cellexciteS.setFillColor(sf::Color(50, 205, 50));
	sf::RectangleShape cellrefractS(sf::Vector2f(sizesquare, sizesquare));
	sf::RectangleShape celldefectS(sf::Vector2f(sizesquare, sizesquare));
	celldefectS.setFillColor(sf::Color(255, 0, 0));

	//**************DRAW HEATMAP SHAPES**************
	sf::CircleShape heathex(radiushex, sides);
	sf::RectangleShape heatsquare(sf::Vector2f(sizesquare, sizesquare));

	//**************SET WINDOW SIZE**************
	if (gridtype == 'h')
	{
		width = (2 * L*apothem) + radiushex;
		height = (1.5*L*radiushex) + (0.5*radiushex);
	}
	else if (gridtype == 's')
	{
		width = widthO;
		height = widthO;
	}
	sf::RenderWindow window(sf::VideoMode(width, height), "AFmodel");
	sf::RenderWindow windowheat(sf::VideoMode(width, height), "Heat Map");
	window.setPosition(sf::Vector2i(width+50, 0));
	windowheat.setPosition(sf::Vector2i(0, 0));

	//**************VISUALISATION STUFF**************
	while (window.isOpen())
	{
		if (visualisation == 'n') window.close();

		while (windowheat.isOpen())
		{
			if (heatmap == 'n') windowheat.close();

			//*********************************************SEVERAL SEEEDS***********************************************
			/*stringstream string1;
			string1 << "H:/fourthyear/Merged/CAModel/Results/GlobalNu=" << globalNu << "_Growth_" << patchGrowthTime;
			if (patchType == 'r') string1 << "_Regular_LocalNu=" << localNu;
			else if (patchType == 'p') string1 << "_Percolation_";
			if (CV == 'y') string1 << "_CV";
			if (gridtype == 'h') string1 << "_HEXAGONAL";
			else if (gridtype == 's') string1 << "_SQUARE";
			string1 << "Seedgroup=" << seedGroup << ".txt";
			string data = string1.str();
			ofstream prisk(data);*/

			//*****************************************************************GROW PATCH****************************************************************
			std::cout << "Nu = " << globalNu << '\t' << "Radius =  " << patchGrowthTime << endl;

			unsigned long long growthSeed = 4318656578904;
			init_genrand64(growthSeed); 
			patchList.push_back(starterCell);		//add starter cell to patch
			int activeCell = starterCell;			//grow starter cell
			activeCellList.push_back(starterCell);	//add starter cell to growable cells

			std::cout << "Beginning to grow patch" << endl;
			for (int t = 0; t < patchGrowthTime; t++)
			{
				//for every 'growable' cell, if conditions satisfied, add to patchList and make active in next iteration
				for (int a = 0; a < activeCellList.size(); a++)
				{
					activeCell = activeCellList[a];	//the growable cell
					lowerLeft = LeftLower(activeCell, L);
					lowerRight = RightLower(activeCell, L);
					upperLeft = LeftUpper(activeCell, L);
					upperRight = RightUpper(activeCell, L);

					//connect to upper left neighbour, don't connect to pacemaker cell
					if (PacemakerCheck(upperLeft, L) == false && PatchCheck(nextActiveCellList, upperLeft) == false && PatchCheck(patchList, upperLeft) == false && genrand64_real2() < patchGrowthProb)
					{
						patchList.push_back(upperLeft);
						nextActiveCellList.push_back(upperLeft);
					}
					//connect to upper right neighbour - if cell is in odd row on last column don't connect (cell doesn't exist)
					if (LastColumnCheck(activeCell, L) == false || ((LastColumnCheck(activeCell, L)) == true && int(floor(activeCell / L)) % 2 == 0))
					{
						if (PatchCheck(nextActiveCellList, upperRight) == false && PatchCheck(patchList, upperRight) == false && genrand64_real2() < patchGrowthProb)
						{
							patchList.push_back(upperRight);
							nextActiveCellList.push_back(upperRight);
						}
					}
					//connect to lower left neighbour, don't grow onto pacemaker cell
					if (PacemakerCheck(lowerLeft, L) == false && PatchCheck(nextActiveCellList, lowerLeft) == false && PatchCheck(patchList, lowerLeft) == false && genrand64_real2() < patchGrowthProb)
					{
						patchList.push_back(lowerLeft);
						nextActiveCellList.push_back(lowerLeft);
					}
					//connect to lower right neighbour - ignore if odd row cell in last column
					if (LastColumnCheck(activeCell, L) == false || ((LastColumnCheck(activeCell, L)) == true && int(floor(activeCell / L)) % 2 == 0))
					{
						if (PatchCheck(nextActiveCellList, lowerRight) == false && PatchCheck(patchList, lowerRight) == false && genrand64_real2() < patchGrowthProb)
						{
							patchList.push_back(lowerRight);
							nextActiveCellList.push_back(lowerRight);
						}
					}
					//check right neighbour - check active cell is not in last column
					if (LastColumnCheck(activeCell, L) == false && PatchCheck(nextActiveCellList, activeCell + 1) == false && PatchCheck(patchList, (activeCell + 1)) == false && genrand64_real2() < patchGrowthProb)
					{
						patchList.push_back(activeCell + 1);
						nextActiveCellList.push_back(activeCell + 1);
					}
					//check left neighbour - check left neighbour is not a pacemaker cell
					if (PacemakerCheck(activeCell - 1, L) == false && PatchCheck(nextActiveCellList, activeCell - 1) == false && PatchCheck(patchList, (activeCell - 1)) == false && genrand64_real2() < patchGrowthProb)
					{
						patchList.push_back(activeCell - 1);
						nextActiveCellList.push_back(activeCell - 1);
					}
					//if cell is in column 1 or cell is in last column (boundaries), push_back perimeter list
					if ((activeCell - 1) % L == 0 || LastColumnCheck(activeCell, L) == true) perimeterList.push_back(activeCell);

				}//close active cell loop

				activeCellList = nextActiveCellList;
				nextActiveCellList.clear();
			}//******************************************************END OF GROWING PATCH**********************************************

			for (int i = 0; i < activeCellList.size(); i++) perimeterList.push_back(activeCellList[i]);
			activeCellList.clear();
			vector<int>().swap(activeCellList);
			vector<int>().swap(nextActiveCellList);

			//*****************************************************SETS OF 10 SEEDS****************************************
			unsigned long long seeds[10];
			unsigned long long seed1[] = { 30676349, 94702886, 33859865, 52508503, 43107877, 55052217, 64613502, 88890983, 51474713, 37606689};
			unsigned long long seed2[] = { 96853379, 32957110, 38823375, 57523667, 73706261, 12365540, 73797677, 98913165, 74976605, 61900305};
			unsigned long long seed3[] = { 78193843, 1215154, 93333423, 69302844, 54857747, 79637993, 1772916, 79137111, 21854250, 17981347};
			unsigned long long seed4[] = { 33556323, 64220881, 10798789, 41718411, 5250194, 32903745, 48169672, 75459097, 41777999, 66360030};
			unsigned long long seed5[] = { 56463460, 1302282, 36365702, 41236007, 41830976, 91088824, 96397296, 92146561, 99808894, 51238447};

			for (int i = 0; i < sizeof(seeds) / sizeof(*seeds); i++)
			{
				if (seedGroup == 1) seeds[i] = seed1[i];
				else if (seedGroup == 2) seeds[i] = seed2[i];
				else if (seedGroup == 3) seeds[i] = seed3[i];
				else if (seedGroup == 4) seeds[i] = seed4[i];
				else if (seedGroup == 5) seeds[i] = seed5[i];
			}


			//unsigned long long seeds[] = { 78193843 };
			//***********************************************************************SEED LOOP***********************************************************************************
			for (int s = 0; s < sizeof(seeds) / sizeof(*seeds); s++)
			{
				cout << seeds[s] << endl;
				_strtime_s(tmpbuf);
				std::cout << tmpbuf << endl;

				c = 0;
				init_genrand64(seeds[s]);
				int discountTime = 0; //discount time if varying conduction velocity

				//**********CREATE LATTICE**********
				int *originalLattice = new int[L*L];
				int *nextLattice = new int[L*L];
				//*********ARRAYS TO STORE DEFECTIVE AND VERTICALLY COUPLED CELLS*********
				int *defectCells = new int[L*L];
				int *excitedCheck = new int[L*L];
				int *timedelay = new int[L*L];
				for (int i = 0; i < (L*L); i++) timedelay[i] = 1;
				int couples[L*L][3] = {};

				std::cout << "Beginning initialisation" << endl;
				//**********************************************************************INITIALISE LATTICE****************************************************************
				for (int i = 0; i < (L*L); i++)
				{
					//if cell is in the patch, change its nu
					if (patchType == 'r' && PatchCheck(patchList, i) == true) nu = localNu;
					else nu = globalNu;

					lowerLeft = LeftLower(i, L);
					lowerRight = RightLower(i, L);

					//***************INITIALISE ARRAYS*****************
					originalLattice[i] = 0;
					nextLattice[i] = 0;
					excitedCheck[i] = 0;
					defectCells[i] = 0;
					couples[i][0] = 0;
					couples[i][1] = 0;
					couples[i][2] = 1;

					//********CLEAR LATTICE & EXCITE PACEMAKER CELLS****
					if (i%L == 0) originalLattice[i] = 1;
					else originalLattice[i] = 0;
					nextLattice[i] = 0;

					//Don't assign dfective cells or transverse couples to pacemaker cells
					if (i%L != 0)
					{
						//***************DEFECTIVE CELLS***************
						if (genrand64_real2() < delta) defectCells[i] = 1;

						//***************LEFT DOWNWARD COUPLING***************
						if (genrand64_real2() < nu)
						{
							if (gridtype == 'h' || int(floor(i / L)) % 2 != 0)
							{
								couples[i][0] = 1;
								timedelay[i]++;
								timedelay[lowerLeft]++;
							}

						}
						//***************RIGHT DOWNWARD COUPLING***************
						if (genrand64_real2() < nu)
						{
							if (gridtype == 'h' || int(floor(i / L)) % 2 == 0)
							{
								couples[i][1] = 1;
								timedelay[i]++;
								timedelay[lowerRight] ++;
							}
						}
					}

					//***************BOUNDARY CONDITIONS***************
					//if cell is in last column, remove right horizontal couples (and right vertical if row is odd)
					if ((i + 1) % L == 0)
					{
						if (int(floor(i / L)) % 2 != 0)
						{
							if (couples[i][1] == 1) timedelay[i]--;
							couples[i][1] = 0;
						}
						couples[i][2] = 0;
					}
					ProgressBar(i, L*L, 1);
				} //END INITIALISE LATTICE

				//PERCOLATION PATCH
				if (patchType == 'p')
				{
					vector<int> cellList, coupleList;
					do
					{
						cellList.clear();
						coupleList.clear();
						for (int i = 0; i < patchList.size(); i++)
						{
							int cell = patchList[i];
							lowerLeft = LeftLower(cell, L);
							lowerRight = RightLower(cell, L);
							//right couple - check cell is not in last column
							if (couples[cell][2] == 1 && LastColumnCheck(cell, L) == false && genrand64_real2() < probabilityIncrement)
							{
								couples[cell][2] = 0;
								timedelay[cell]--;
								timedelay[cell + 1]--;
								cellList.push_back(cell);
								coupleList.push_back(2);
							}
							//lower left couple
							if (couples[cell][0] == 1 && genrand64_real2() < probabilityIncrement)
							{
								couples[cell][0] = 0;
								timedelay[cell]--;
								timedelay[lowerLeft]--;
								cellList.push_back(cell);
								coupleList.push_back(0);
							}
							//lower right, only apply if cell has a lower right couple
							if (LastColumnCheck(cell, L) == false || (LastColumnCheck(cell, L) == true && int(floor(cell % L)) % 2 == 0))
							{
								if (couples[cell][1] == 1 && genrand64_real2() < probabilityIncrement)
								{
									couples[cell][1] = 0;
									timedelay[cell]--;
									timedelay[lowerRight]--;
									cellList.push_back(cell);
									coupleList.push_back(1);
								}
							}
						}
					} while (PercolationCheck(patchList, perimeterList, starterCell, couples, L, percPercentage) == true);

					for (int i = 0; i < cellList.size(); i++)
					{
						lowerLeft = LeftLower(cellList[i], L);
						lowerRight = RightLower(cellList[i], L);
						timedelay[cellList[i]]++;
						if (coupleList[i] == 0) timedelay[lowerLeft]++;
						else if (coupleList[i] == 1) timedelay[lowerRight]++;
						else if (coupleList[i] == 2) timedelay[cellList[i] + 1];
						couples[cellList[i]][coupleList[i]] = 1;
					}
					cellList.clear();
					coupleList.clear();
					vector<int>().swap(cellList);
					vector<int>().swap(coupleList);

					double couplingPercProb = 0.0;
					for (int i = 0; i < patchList.size(); i++)
					{
						if (couples[patchList[i]][0] == 1) couplingPercProb++;
						if (couples[patchList[i]][1] == 1) couplingPercProb++;
						if (couples[patchList[i]][2] == 1) couplingPercProb++;
					}
					couplingPercProb = couplingPercProb / (patchList.size() * 3);
					std::cout << "coupling probability within patch = " << couplingPercProb << endl;

					double averageTimeDelay = 0;
					for (int i = 0; i < L*L; i++) if (PacemakerCheck(i, L) == false && LastColumnCheck(i, L) == false && PatchCheck(patchList, i) == false) averageTimeDelay += timedelay[i];
					averageTimeDelay = averageTimeDelay / (L*L - 2 * L - patchList.size());
				}//************************************************************************END PERCOLATION PATCH******************************************************

				//******************************************************************************DRAW HEATMAP*************************************************************
				for (int i = 0; i < (L*L); i++)
				{
					if (heatmap == 'y')
					{
						if (gridtype == 'h')
						{
							heathex.setFillColor(sf::Color(HeatRed(timedelay[i], gridtype), HeatGreen(timedelay[i], gridtype), HeatBlue(timedelay[i], gridtype)));
							heathex.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
							windowheat.draw(heathex);
						}

						else if (gridtype == 's')
						{
							heatsquare.setFillColor(sf::Color(HeatRed(timedelay[i], gridtype), HeatGreen(timedelay[i], gridtype), HeatBlue(timedelay[i], gridtype)));
							heatsquare.setPosition(sizesquare*(i%L), sizesquare*floor(i / L));
							windowheat.draw(heatsquare);
						}
					}
				}

				//******************************************OUTPUT HEATMAP**********************************************
				if (screenshot == 'y' && heatmap == 'y')
				{
					stringstream Kishan;
					Kishan << "H:/fourthyear/Merged/CAModel/Results/Screenshots/HEATMAP_GlobalNu=" << globalNu;
					if (patchType == 'r') Kishan << "Regular_LocalNu=" << localNu;
					else if (patchType == 'p') Kishan << "Percolation_";
					if (CV == 'y') Kishan << "_CV";
					if (gridtype == 'h') Kishan << "_HEXAGONAL";
					else if (gridtype == 's') Kishan << "_SQUARE";
					Kishan << "Seed=" << seeds[s] << ".png";
					string heatdata = Kishan.str();
					sf::Image heatscreenshot = windowheat.capture();
					heatscreenshot.saveToFile(Kishan.str());
				}
				if (heatmap == 'y') windowheat.display();
				//********************************************************************END HEATMAP*******************************************

				//********************************EXAGGERATE TIMEDELAY********************************
				if (CV == 'n') for (int i = 0; i < (L*L); i++) timedelay[i] = 1;

				if (CV == 'y')
				{
					if (exaggerate == 'y')
					{
						for (int i = 0; i < (L*L); i++)
						{
							int exaggerateTimeDelay = timedelay[i] * factor - (factor - 1);
							timedelay[i] = exaggerateTimeDelay;
						}
					}

					//****************SCALE TMAX AND SAP AND DISCOUNT TIME****************
					for (int i = 0; i < (L*L); i++) averagetimedelay += timedelay[i];
					averagetimedelay = averagetimedelay / (L*L);
					tmax = tmax*averagetimedelay;
					SAP = SAP*averagetimedelay;
					discountTime = 40 * SAP; //discount first 40 SAPs
				}

				std::cout << "Beginning time loop" << endl;

				//*********************************************************TIME LOOP**************************************************
				for (int t = 0; t < tmax; t++)
				{
					window.clear();

					//*************************************************LATTICE CELL LOOP**********************************************
					for (int i = 0; i < L*L; i++)
					{
						if (visualisation == 'y')
						{
							a = floor(double(i) / double(L));
							b = i%L;
						}

						lowerLeft = LeftLower(i, L);
						lowerRight = RightLower(i, L);
						upperRight = RightUpper(i, L);
						upperLeft = LeftUpper(i, L);

						//IGNORE FIRST COLUMN (PACEMAKER CELLS)
						if (i%L != 0 && originalLattice[i] == 1)
						{
							//cells that have been excited in previous time step, start refractory period
							nextLattice[i] = (-1)*refp;

							//***********LEFT DIAGONAL EXCITATION (LOWER AND UPPER)***********
							if (couples[i][0] == 1 && excitedCheck[lowerLeft] == 0 && originalLattice[lowerLeft] == 0)
							{
								if (defectCells[lowerLeft] == 1 && (genrand64_real2() < epsilon)) nextLattice[lowerLeft] = 0;
								else nextLattice[lowerLeft] = timedelay[lowerLeft];
								excitedCheck[lowerLeft] = 1;
							}
							if (couples[upperLeft][1] == 1 && excitedCheck[upperLeft] == 0 && originalLattice[upperLeft] == 0)
							{
								if (defectCells[upperLeft] == 1 && (genrand64_real2() < epsilon)) nextLattice[upperLeft] = 0;
								else nextLattice[upperLeft] = timedelay[upperLeft];
								excitedCheck[upperLeft] = 1;
							}

							//*********RIGHT EXCITATION (LOWER AND UPPER)***********
							if (couples[i][1] == 1 && excitedCheck[lowerRight] == 0 && originalLattice[lowerRight] == 0)
							{
								if (defectCells[lowerRight] == 1 && (genrand64_real2() < epsilon)) nextLattice[lowerRight] = 0;
								else nextLattice[lowerRight] = timedelay[lowerRight];
								excitedCheck[lowerRight] = 1;
							}

							if (couples[upperRight][0] == 1 && excitedCheck[upperRight] == 0 && originalLattice[upperRight] == 0)
							{
								if (defectCells[upperRight] == 1 && (genrand64_real2() < epsilon)) nextLattice[upperRight] = 0;
								else nextLattice[upperRight] = timedelay[upperRight];
								excitedCheck[upperRight] = 1;
							}

							//***************HORIZONTAL LEFT NEIGHBOURS************
							if (couples[i - 1][2] == 1 && excitedCheck[i] == 0 && originalLattice[i - 1] == 0)
							{
								if (defectCells[i - 1] == 1 && (genrand64_real2() < epsilon)) nextLattice[i - 1] = 0;
								else nextLattice[i - 1] = timedelay[i - 1];
								excitedCheck[i - 1] = 1;
							}

							if (visualisation == 'y'&& t >= timeStart)
							{
								if (gridtype == 'h')
								{
									cellexciteH.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
									window.draw(cellexciteH);
								}

								else if (gridtype == 's')
								{
									cellexciteS.setPosition(sizesquare*b, sizesquare*a);
									window.draw(cellexciteS);
								}
							}

						} //close ignore-first-column if loop

						//************CHECK RIGHT NEIGHBOURS************ 
						if (originalLattice[i] == 1 && couples[i][2] == 1 && excitedCheck[i + 1] == 0 && originalLattice[i + 1] == 0)
						{
							if (defectCells[i + 1] == 1 && (genrand64_real2() < epsilon)) nextLattice[i + 1] = 0;
							else nextLattice[i + 1] = timedelay[i + 1];
							excitedCheck[i + 1] = 1;
						}

						//***************UPDATE REFRACTORY AND TIMEDELAYED CELL STATES AND DRAW CELLS***************
						//increment cells that are already refractory by 1
						else if (originalLattice[i] < 0)
						{
							nextLattice[i] = originalLattice[i] + 1;

							if (visualisation == 'y'&& t >= timeStart)
							{
								double greyshade = abs(greyshadeinc*originalLattice[i]);
								double red = abs(redinc*nextLattice[i]);
								double green = abs(greeninc*nextLattice[i]);
								double blue = abs(blueinc*nextLattice[i]);

								if (gridtype == 'h')
								{
									cellrefractH.setFillColor(sf::Color(red, green, blue));
									cellrefractH.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
									window.draw(cellrefractH);
								}

								else if (gridtype == 's')
								{
									cellrefractS.setFillColor(sf::Color(red, green, blue));
									cellrefractS.setPosition(sizesquare*b, sizesquare*a);
									window.draw(cellrefractS);
								}
							}
						}
						//decrease cells that are in the 'timedelay' state by 1
						else if (originalLattice[i] > 1 && i % L != 0) nextLattice[i] = originalLattice[i] - 1;

						//***************************DRAW DEFECTIVE CELLS************************
						else if (defectCells[i] == 1 && visualisation == 'y'&& t >= timeStart)
						{
							if (gridtype == 'h')
							{
								celldefectH.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
								window.draw(celldefectH);
							}

							else if (gridtype == 's')
							{
								celldefectS.setPosition(sizesquare*b, sizesquare*a);
								window.draw(celldefectS);
							}
						}

						//*******************************UPDATE PACEMAKER CELL STATE******************************
						if (i%L == 0)
						{
							//if pacemaker cell is excited, make refractory in the next time step
							if (originalLattice[i] == 1) nextLattice[i] = SAP + 1;
							//increment refractory states by 1 (if not already in the last refractory state "-1"
							else if (originalLattice[i] > 1) nextLattice[i] = originalLattice[i] - 1;
						}

					} //************************************************************END LATTICE LOOP**************************************************

					//***********UPDATE TIMESTEP***********
					for (int i = 0; i<L*L; i++)
					{
						if (nextLattice[i] == 1) ecount++;
						originalLattice[i] = nextLattice[i];
						nextLattice[i] = 0;
						excitedCheck[i] = 0;
					}

					//*****CHECK IF SYSTEM IS FIBRILLATORY*****
					if (t >= discountTime && ecount > fib_thresh)
					{
						AFtimecount++;
						if (AFtimecount == 1) std::cout << t << endl;
					}

					//cout << t << "\r" << flush;
					ecount = 0;

					//***************PROGRESS BAR****************
					//ProgressBar(t, tmax, 1);
					if (t == tmax - 1)
					{
						std::cout << "seed " << s + 1 << " completed" << endl;
						_strtime_s(tmpbuf);
						std::cout << tmpbuf << endl;
					}

					//********************************OUTPUT VISUALISATION & SCREENSHOTS***********************************
					if (visualisation == 'y' && t >= timeStart) window.display();
					if (screenshot == 'y' && t >= timeStart)
					{
						stringstream VisualSequenceString;
						VisualSequenceString << "H:/fourthyear/Merged/CAModel/Results/Screenshots/Homo/TESTSHOT_GlobalNu=" << globalNu;
						if (patchType == 'r') VisualSequenceString << "Regular_LocalNu=" << localNu;
						else if (patchType == 'p') VisualSequenceString << "Percolation_";
						if (CV == 'y') VisualSequenceString << "_CV";
						if (gridtype == 'h') VisualSequenceString << "_HEXAGONAL";
						else if (gridtype == 's') VisualSequenceString << "_SQUARE";
						VisualSequenceString << "Seed=" << seeds[s] << "_StartTime=" << timeStart << "_" << setfill('0') << setw(4) << c << ".png";
						string data = VisualSequenceString.str();
						sf::Image screenshot = window.capture();
						screenshot.saveToFile(VisualSequenceString.str());
						c++;
						if (c == 2000) break;
					}

				}//************************************************************END TIME LOOP************************************************************

				_strtime_s(tmpbuf);

				//*****OUTPUT TO FILE PROB RISK********
				double p_risk = double(AFtimecount) / double(tmax - discountTime);
				//prisk << patchGrowthTime << '\t' << patchCol << '\t' << localNu << '\t' << p_risk << '\t' << seeds[s] << endl;
				//prisk << nu << '\t' << p_risk << '\t' << seeds[s] << endl;
				//std::cout << localNu << '\t' << p_risk << '\t' << seeds[s] << '\t' << tmpbuf << endl;
				AFtimecount = 0;

				delete[] originalLattice;
				delete[] nextLattice;
				delete[] defectCells;
				delete[] excitedCheck;
				delete[] timedelay;

			} //*****************************************************END SEED LOOP***********************************************************************
		} //close while heat loop

	}//close while loop

} //close main-loop