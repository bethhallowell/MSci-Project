#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <string>  
#include <cmath>  
#include <math.h>
#include <stdio.h> 
#include <vector>
#include <algorithm>
#include <ctime>
#include <time.h>
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

	string CV;
	std::cout << "" << endl;
	std::cout << "Do you want to vary conduction velocity? (y/n) " << endl;
	cin >> CV;

	string exaggerate;
	if (CV == "y")
	{
		std::cout << "" << endl;
		std::cout << "Do you also want to exaggerate the variation? (y/n) " << endl;
		cin >> exaggerate;
		tmax = 100000;
	}

	int factor;
	//string power;
	if (exaggerate == "y")
	{
		std::cout << "" << endl;
		std::cout << "By what factor do you want to exaggerate the variation? Insert number" << endl;
		cin >> factor;
	}

	double widthO = 1200;
	double radiushex = double(widthO / 2) / double(L);
	double sides = 6;
	double sizesquare = double(widthO) / double(L);
	double apothem = radiushex*cos(pi / (sides));
	double horizontal_shift = radiushex - apothem;
	double vertical_shift = radiushex / 2;
	double greyshadeinc = floor(255 / refp), redinc = floor(0 / refp), greeninc = floor(191 / refp), blueinc = floor(255 / refp);
	int width, height;


	string visualisation;
	cout << "visualisation? (y/n) " << endl;
	cin >> visualisation;

	width = (2 * L*apothem) + radiushex;
	height = (1.5*L*radiushex) + (0.5*radiushex);

	sf::RenderWindow window(sf::VideoMode(width, height), "test");
	window.setPosition(sf::Vector2i(500, 0));
	



	////*****INITIALISE NU VARIABLES****
	double nu_inc = 0.1; //increments of nu
	double nu_low = 0.3, nu_high = 1.0;
	double nu_max = 1.0 + ((nu_high - nu_low) / nu_inc); //number of nu loops to run from 0 to 1(add 1 to include nu=0)
	//double nu = 0.0; //***ONLY IF TESTING OUTSIDE OF NU LOOP
	double delta = 0.05;        //prob of defective cells
	double epsilon = 0.05;      //prob of cell not exciting
	double fib_thresh = L*1.2;  //threshold of number of excited cells to define 'in AF' state
	double averagetimedelay = 0.0;
	/*unsigned long long singleseed = 93487539489124;
	init_genrand64(singleseed);*/

	while (window.isOpen())
	{
		if (visualisation == "n") window.close();

		double temp[80] = { 0 };
		double data[40][2] = { 0 };
		float a;
		ifstream myfile;
		myfile.open("H:/fourthyear/CriticalRegions/AverageCriticalRegions.txt");
		
		for (int i = 0; i < 80; i++)
		{
			myfile >> temp[i];	//pass all values in text file to temp array
		}
		myfile.close();

		for (int i = 0; i < 80; i++)
		{
			if (i % 2 == 0) data[i / 2][0] = temp[i];
			if (i % 2 != 0) data[i / 2 ][1] = temp[i];
		}
		
		//for (int i = 0; i < 40; i++)
		//{
		//	cout <<  data[i][0] << '\t' << data[i][1] << endl;
		//	//system("pause");
		//}

		stringstream string2;
		string2 << "H:/fourthyear/CriticalRegions/NKminKmax.txt";
		if (CV == "y") string2 << "_CV_Factor" << factor;
		string2 << ".txt";
		string data2 = string2.str();
		ofstream criticaldata(data2);

		//stringstream string3;
		//string3 << "H:/fourthyear/Merged/CAModel/Results/MinAvgCountData";
		//if (CV == "y") string3 << "_CV_Factor" << factor;
		//string3 << ".txt";
		//string data3 = string3.str();
		//ofstream mindata(data3);

		for (int n = 0; n < nu_max; n++)
		{

			//*****INITIALISE NU LOOP VARIABLES***** 
			double nu = nu_low;
			nu = nu + (nu_inc * n);
			cout << "nu = " << nu << endl;

			int N;
			for (int k = 0; k < 40; k++)
			{
				if (data[k][0] == nu)
				{
					N = ceil(data[k][1]);
					//cout << "nu = " << data[k][0] << " count = " << N << endl;
				}
			}
			//*******DECLARE CONSTANTS*******
			//300 seeds
			unsigned long long seeds[] = { 802636682, 808474665, 257612017, 483910152, 700820597, 303094561, 759153659, 285963902, 167839038, 836936710, 740674320, 11104793, 879420803, 845390401, 918869570, 445391285, 329066218, 647070118, 148508391, 746140206, 168100268, 531671409, 151802970, 482084825, 840544092, 515337622, 132308160, 855985885, 647000845, 725488849, 174768261, 756531850, 105893336, 191400116, 971004720, 292845380, 59903363, 899102155, 59865246, 114341407, 814112183, 836698750, 733263042, 498955561, 875236607, 152589041, 86988714, 499290887, 535781365, 290421117, 254128834, 862699763, 399586719, 919099901, 651024042, 869380131, 730160658, 269010768, 423539986, 718288526, 333950325, 79219020, 979740516, 684294926, 449324627, 617478322, 399658866, 263406538, 689754651, 62725189, 211939543, 804565854, 103436225, 422763402, 935859433, 674813884, 866320871, 492364649, 809329175, 989956526, 208491986, 164327967, 522342484, 763859374, 249496180, 520321959, 135534308, 213582797, 192152779, 522811162, 581429122, 546462232, 847634127, 160658017, 465787242, 916506684, 203875449, 881012565, 146503082, 447755424, 641580149, 715135242, 976761231, 316950194, 802524514, 883994399, 449953497, 851283555, 294471384, 271866177, 472024969, 93633120, 628277657, 526138430, 718638921, 408104623, 653743868, 544904083, 620723004, 248416050, 461801140, 7278945, 637105003, 120050641, 937622250, 684413412, 458656650, 941112047, 728712796, 344012562, 649822948, 883748229, 434648848, 706140402, 9416875, 598671200, 572298006, 305138621, 210082957, 274947658, 876219743, 991977126, 385892298, 350267023, 711043425, 404446473, 35845309, 275262201, 905391179, 64716056, 935335607, 734226403, 727442471, 671898805, 253069505, 86482783, 372178339, 240666364, 749235849, 48089246, 38032263, 281712230, 858179713, 99099130, 936407905, 643199440, 923108681, 372224959, 927290466, 89854521, 535262851, 434991637, 786798320, 353484382, 425375181, 894987097, 16911061, 999798846, 978162886, 491071734, 396388525, 400849114, 857388963, 663565381, 260951011, 528617441, 551297301, 555713925, 727222179, 432431014, 819621871, 744920167, 353678864, 998791924, 112490345, 169831232, 644174169, 521482948, 120947504, 687868502, 975479602, 175240360, 608994961, 379318225, 636238634, 975722418, 432540788, 347600831, 486031538, 995205243, 184062933, 688808516, 590902216, 773018219, 826096343, 968294478, 544616683, 133803079, 692370051, 215592882, 681284459, 599801596, 430414283, 44561378, 976165651, 237446060, 436871365, 313019587, 446471924, 576146414, 965026444, 712909326, 748403991, 525893922, 985146014, 369396191, 872053437, 659312618, 663381736, 650236238, 221663828, 525199441, 600049997, 862129803, 151881331, 662804326, 566650964, 378531907, 978980021, 798052489, 916235838, 355553329, 166070571, 107832213, 684856902, 295397994, 54465175, 237187322, 625340257, 364356381, 527129990, 730289882, 747088128, 845894648, 409672140, 328036566, 995370410, 829560826, 754220012, 380952239, 310867904, 967384494, 319036708, 282290915, 985057394, 88749630, 921636286, 266591959, 375595757, 36553307, 591018718, 745012772, 432979987, 687221821, 506166192, 181286998, 406726240, 761540266, 820385246, 320773278, 766747348, 71170928, 742426715, 897047935, 322067054, 768793717, 612103313, 594562470, 466931712, 561297732 };
			int ecount = 0, AFtimecount = 0;

			//**********************SEED LOOP**********************************
			for (int s = 0; s < sizeof(seeds) / sizeof(*seeds); s++)
			{
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
				int diagonalCouples[L*L][2] = {};

				//***********INITIALISE LATTICE***********
				for (int i = 0; i < (L*L); i++)
				{
					//***************INITIALISE ARRAYS*****************
					originalLattice[i] = 0;
					nextLattice[i] = 0;
					excitedCheck[i] = 0;

					//****CLEAR LATTICE & EXCITE PACEMAKER CELLS****
					if (i%L == 0) originalLattice[i] = 1;
					else originalLattice[i] = 0;
					nextLattice[i] = 0;

					//***********DEFECTIVE*********
					if (genrand64_real2() < delta) defectCells[i] = 1;
					else defectCells[i] = 0;

					//**********LEFT COUPLING**********
					if (genrand64_real2() < nu)
					{
						//IF CELL IS IN EVEN ROW 
						if ((int(floor(double(i) / double(L))) % 2) == 0)
						{
							//IF CELL IS NOT IN FIRST COLUMN
							if ((i % L) != 0)
							{
								//CREATE LEFT DOWNWARD COUPLING
								diagonalCouples[i][0] = 1;
								timedelay[i + L - 1]++;
							}
						} //END EVEN IF STATEMENT

						//ODD ROWS FOR LEFT COUPLING
						else if ((int(floor(double(i) / double(L))) % 2) != 0)
						{
							//IF CELL IS IN LAST ROW - SPECIAL CASE: COUPLE TO FIRST ROW (LAST ROW IS ALWAYS ODD)
							if (floor(double(i) / double(L)) == (L - 1))
							{
								diagonalCouples[i][0] = 1;
								timedelay[i % L]++;
							}
							else //CREATE LEFT DOWNWARD COUPLING
							{
								diagonalCouples[i][0] = 1;
								timedelay[i + L]++;
							}
						}

						if (CV == "y") timedelay[i]++;
					}
					else diagonalCouples[i][0] = 0;
					//***************END LEFT DIAGONAL COUPLING************************

					//***********RIGHT COUPLING***********
					if (genrand64_real2() < nu)
					{
						//IF CELL IS IN EVEN ROW
						if ((int(floor(double(i) / double(L))) % 2) == 0)
						{
							//CREATE RIGHT DOWNWARD COUPLING
							diagonalCouples[i][1] = 1;
							timedelay[i + L]++;
						}

						//ELSE IF CELL IS IN ODD ROW FOR RIGHT COUPLING
						else if ((int(floor(double(i) / double(L))) % 2) != 0)
						{
							//AND CELL IS NOT IN LAST COLUMN 
							if ((i + 1) % L != 0)
							{
								//IF CELL IS IN LAST ROW - SPECIAL CASE: COUPLE TO FIRST ROW (LAST ROW IS ALWAYS ODD)
								if (floor(double(i) / double(L)) == (L - 1))
								{
									diagonalCouples[i][1] = 1;
									timedelay[(i % L) + 1]++;
								}
								else
								{
									diagonalCouples[i][1] = 1;
									timedelay[i + L + 1]++;
								}
							}
						}
						timedelay[i]++;
					}
					else diagonalCouples[i][1] = 0;
					//***********************END RIGHT COUPLING**************************

					//********BOUNDARY CONDITIONS*********
					if ((i + 1) % L == 0 && int(floor(i / L)) % 2 != 0) diagonalCouples[i][1] = 0;
					if (i % L == 0) defectCells[i] = 0;

				} // close create defective and coupling cell for loop


				vector<int> timeDelayVector;
				if (CV == "y")
				{
					if (exaggerate == "y")
					{
						for (int i = 0; i< (L*L); i++)
						{
							//********EXAGGERATE TIMEDELAY*******
							//int exaggerateTimeDelay = pow(timedelay[i], 2);
							int exaggerateTimeDelay = timedelay[i] * factor - (factor - 1);
							timedelay[i] = exaggerateTimeDelay;
							if (i % L != 0 && i % L != (L - 1))
							{
								timeDelayVector.push_back(timedelay[i]);//do we want diagonalCouples or timedelay????
							}
						}
					}
				}

				vector<int> counts;
				int kmin=0; // = tau effective with hetero
				int kmax=0; // = sum of all k over N

				//**************************************************FIND KMIN AND KMAX********************************************************
				//for (int c = 0; c < N; c++)
				//{
				//	int timeDelayCounter = 0, counter = 0;
				//	do
				//	{
				//		int randomCell = genrand64_real2()*timeDelayVector.size();
				//		timeDelayCounter += timeDelayVector[randomCell];
				//		counter++;
				//		timeDelayVector.erase(timeDelayVector.begin() + randomCell);
				//		
				//	} while (timeDelayCounter < refp);
				//	counts.push_back(counter);
				//}
				//kmin = *std::min_element(counts.begin(), counts.end());
				//kmax = *std::max_element(counts.begin(), counts.end());
				///*int kcount = 0;
				//if (counts.size() != 0)
				//{
				//	for (int k = 0; k < N; k++) kcount += counts[k];
				//	kavg = kcount / counts.size();
				//}*/
				//mindata << nu << '\t' << kmin << '\t' <<  kmax << '\t' << N << '\t' << seeds[s] << endl;
				//kmax = 0;
				//kmin = 0;
				//counts.clear();
				//timeDelayVector.clear();
				//////*****FIND KMIN/KMAX*******
				//ProgressBar(s, sizeof(seeds) / sizeof(*seeds), 1);

				
				
				//******************************************************CRITICAL ROTOR DETECTION*************************************************************
				
				////************DRAW HEXAGONS*************
				sf::CircleShape cell(radiushex, sides);
				cell.setFillColor(sf::Color(153, 255, 153));	//set all cells to green
				cell.setOutlineThickness(1);
				cell.setOutlineColor(sf::Color(0, 0, 0));

				sf::CircleShape celldefect(radiushex, sides);
				celldefect.setFillColor(sf::Color(255, 0, 0));

				sf::CircleShape connected(radiushex, sides);
				connected.setFillColor(sf::Color(0, 0, 255));

				sf::CircleShape celldefect2(radiushex, sides);
				celldefect2.setFillColor(sf::Color(255, 255, 255));

				vector<int> criticalDefects;
				int delayCount;
				//bool critical = false;

				int counter = 0;
				//DRAW ENTIRE LATTICE
				if (visualisation == "y")
				{
					for (int i = 0; i < (L*L); i++)
					{
						cell.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
						window.draw(cell);
					}
				}

				for (int i = 0; i < L * L; i++)
				{
					delayCount = 0;

					if (defectCells[i] == 1)
					{
						//cout << i << endl;
						//===========================================DOWNWARD ROTORS======================================================
						//******FORWARDS*****
						counter = 0;
						int j = i;
						int columnj = j % L;

						do
						{
							if (visualisation == "y")
							{
								for (int i = 0; i < (L*L); i++)
								{
									cell.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
									if (defectCells[i] == 1) cell.setFillColor(sf::Color(255, 153, 153));
									else cell.setFillColor(sf::Color(153, 255, 153));
									window.draw(cell);
								}
								celldefect2.setPosition(GetX(j, L, widthO), GetY(j, L, widthO));
								window.draw(celldefect2);
								window.display();
								//system("pause");
							}

							/*cout << j << '\t' << LeftLower(j, L) << '\t' << diagonalCouples[j][0] << endl;
							cout << j << '\t' << RightLower(j, L) << '\t' << diagonalCouples[j][1] << endl;*/

							delayCount += timedelay[j];
							counter++;
							//cout << j << '\t' << timedelay[j] << '\t' << delayCount << endl;
							if ((j + 1) % L == 0) break;
							j++;
							if (defectCells[j] == 1 && diagonalCouples[j][0] == 0 && diagonalCouples[j][1] == 0) break;

						} while (diagonalCouples[j][0] == 0 && diagonalCouples[j][1] == 0); //AND - only continue if NO connections existed 
						
						if ((j + 1) % L == 0) goto upward;	//skips the rest of the downward rotor check if cell is odd and in last column (no right couples exist)
						if (defectCells[j] == 1 && diagonalCouples[j][0] == 0 && diagonalCouples[j][1] == 0) continue;
						delayCount += timedelay[j];
						counter++;
						//cout << j << '\t' << timedelay[j] << '\t' << delayCount << endl;

						//**************PASS DOWN**************
						if (visualisation == "y")
						{
							for (int i = 0; i < (L*L); i++)
							{
								cell.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
								if (defectCells[i] == 1) cell.setFillColor(sf::Color(255, 153, 153));
								else cell.setFillColor(sf::Color(153, 255, 153));
								window.draw(cell);
							}
						}

						int k = j;

						if (diagonalCouples[j][1] == 1)  k = RightLower(j, L);	//check right couple first as j will be passed to the left couple (need left couple to be priority)
						if (diagonalCouples[j][0] == 1) k = LeftLower(j, L);
						

						if (visualisation == "y")
						{
							connected.setPosition(GetX(j, L, widthO), GetY(j, L, widthO));
							window.draw(connected);
							window.display();
						}
						//system("pause");


						//*******BACKWARDS***********
						do
						{
							if (visualisation == "y")
							{
								for (int i = 0; i < (L*L); i++)
								{
									cell.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
									if (defectCells[i] == 1) cell.setFillColor(sf::Color(255, 153, 153));
									else cell.setFillColor(sf::Color(153, 255, 153));
									window.draw(cell);
								}
								celldefect2.setPosition(GetX(k, L, widthO), GetY(k, L, widthO));
								window.draw(celldefect2);
								window.display();
							}
							//system("pause");


							delayCount += timedelay[k];
							counter++;
							//cout << k << '\t' << timedelay[k] << '\t' << delayCount << endl;
							k--;
							/*	cout << k << '\t' << LeftUpper(k, L) << '\t' << diagonalCouples[RightUpper(k, L)][0] << endl;
							cout << k << '\t' << RightUpper(k, L) << '\t' << diagonalCouples[LeftUpper(k, L)][1] << endl;
							//system("pause");*/

						} while ((k + 1) % L != columnj);
						k++;



						//********FINAL DOWNWARD VISUALISATION*********
						if (visualisation == "y")
						{
							for (int i = 0; i < (L*L); i++)
							{
								cell.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
								if (defectCells[i] == 1) cell.setFillColor(sf::Color(255, 153, 153));
								else cell.setFillColor(sf::Color(153, 255, 153));
								window.draw(cell);
							}

							celldefect.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
							window.draw(celldefect);

							connected.setPosition(GetX(j, L, widthO), GetY(j, L, widthO));
							window.draw(connected);

							celldefect2.setPosition(GetX(k, L, widthO), GetY(k, L, widthO));
							window.draw(celldefect2);

							window.display();
							//system("pause");
						}

						if (delayCount > refp)
						{
							criticalDefects.push_back(i);
							counts.push_back(counter);
							//critical = true;
						}
		
						//================================================UPWARD ROTORS================================================
						//******FORWARDS*****
						
					upward:
						counter = 0;
						delayCount = 0;
						j = i;
						do
						{
							if (visualisation == "y")
							{
								for (int i = 0; i < (L*L); i++)
								{
									cell.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
									if (defectCells[i] == 1) cell.setFillColor(sf::Color(255, 153, 153));
									else cell.setFillColor(sf::Color(153, 255, 153));
									window.draw(cell);
								}
								celldefect2.setPosition(GetX(j, L, widthO), GetY(j, L, widthO));
								window.draw(celldefect2);
								window.display();
								//system("pause");
							}

							/*cout << j << '\t' << LeftUpper(j, L) << '\t' << diagonalCouples[LeftUpper(j, L)][1] << endl;
							cout << j << '\t' << RightUpper(j, L) << '\t' << diagonalCouples[RightUpper(j, L)][0] << endl;*/

							delayCount += timedelay[j];
							counter++;
							//cout << j << '\t' << timedelay[j] << '\t' << delayCount << endl;
							if ((j + 1) % L == 0) break;
							j++;
							if (defectCells[j] == 1 && diagonalCouples[LeftUpper(j, L)][1] == 0 && diagonalCouples[RightUpper(j, L)][0] == 0) break;

						} while (diagonalCouples[LeftUpper(j, L)][1] == 0 && diagonalCouples[RightUpper(j, L)][0] == 0);	//AND - only want to continue if NO connections existed, it was only connecting to cells with 2 connections when using OR

						if ((j + 1) % L == 0) continue;	//skips the rest of the upward rotor check if cell is in last column (no right couples exist)
						if (defectCells[j] == 1 && diagonalCouples[LeftUpper(j, L)][1] == 0 && diagonalCouples[RightUpper(j, L)][0] == 0) continue;
						delayCount += timedelay[j];
						counter++;
						//cout << j << '\t' << timedelay[j] << '\t' << delayCount << endl;

						//**************PASS UPWARD**************
						if (visualisation == "y")
						{
							for (int i = 0; i < (L*L); i++)
							{
								cell.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
								if (defectCells[i] == 1) cell.setFillColor(sf::Color(255, 153, 153));
								else cell.setFillColor(sf::Color(153, 255, 153));
								window.draw(cell);
							}
						}

						k = j;
						/*cout << j << '\t' << LeftUpper(j, L) << '\t' << diagonalCouples[LeftUpper(j, L)][1] << endl;
						cout << j << '\t' << RightUpper(j, L) << '\t' << diagonalCouples[RightUpper(j, L)][0] << endl;*/

						if (diagonalCouples[RightUpper(j, L)][0] == 1) k = RightUpper(j, L);	//check right upper first so that the left value takes priority
						if (diagonalCouples[LeftUpper(j, L)][1] == 1) k = LeftUpper(j, L);

						if (visualisation == "y")
						{
							connected.setPosition(GetX(j, L, widthO), GetY(j, L, widthO));
							window.draw(connected);
							window.display();
							//system("pause");
						}


						//*******BACKWARDS***********
						do
						{
							if (visualisation == "y")
							{
								for (int i = 0; i < (L*L); i++)
								{
									cell.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
									if (defectCells[i] == 1) cell.setFillColor(sf::Color(255, 153, 153));
									else cell.setFillColor(sf::Color(153, 255, 153));
									window.draw(cell);
								}
								celldefect2.setPosition(GetX(k, L, widthO), GetY(k, L, widthO));
								window.draw(celldefect2);
								window.display();
								//system("pause");
							}

							delayCount += timedelay[k];
							counter++;
							//cout << k << '\t' << timedelay[k] << '\t' << delayCount << endl;

							k--;

						} while ((k + 1) % L != columnj);
						k++;

						//********FINAL UPWARD VISUALISATION*********
						if (visualisation == "y")
						{
							for (int i = 0; i < (L*L); i++)
							{
								cell.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
								if (defectCells[i] == 1) cell.setFillColor(sf::Color(255, 153, 153));
								else cell.setFillColor(sf::Color(153, 255, 153));
								window.draw(cell);
							}

							celldefect.setPosition(GetX(i, L, widthO), GetY(i, L, widthO));
							window.draw(celldefect);

							connected.setPosition(GetX(j, L, widthO), GetY(j, L, widthO));
							window.draw(connected);

							celldefect2.setPosition(GetX(k, L, widthO), GetY(k, L, widthO));
							window.draw(celldefect2);

							window.display();
							//system("pause");
						}
						if (delayCount > refp)
						{
							/*if(critical == false)*/ criticalDefects.push_back(i);
							counts.push_back(counter);
						}
						counter = 0;
					} //close 'if-defective' loop

				}//close lattice scan


				
			//	cout << nu << '\t'<<  criticalDefects.size() << endl;

				
				criticaldata << nu << '\t' << criticalDefects.size() <<'\t' << seeds[s] << endl;
				criticalDefects.clear();

				delete[] originalLattice;
				delete[] nextLattice;
				delete[] excitedCheck;
				delete[] timedelay;
				delete[] defectCells;
			//	//ProgressBar(s, sizeof(seeds) / sizeof(*seeds), 1);

			} //close seed-loop
			//criticaldata.close();
		}//close nu loop
	}
} //close main-loop