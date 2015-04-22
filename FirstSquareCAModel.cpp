#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <string>  
#include <cmath>  
#include <math.h>
#include <stdio.h> 
#include <vector>
 
#include "mt64.h"
#include "HelperFunctions.h"
#include <SFML/Graphics.hpp> 
 
using namespace std;  
 
 
#define L 200 //lattice size
#define refp 50 //refractory period
#define SAP 220 //period of pacemaker cells
#define tmax 1000000
 
 
void main()
{
    ////*****INITIALISE NU VARIABLES****
    double nu_inc = 0.01; //increments of nu
    double nu_high = 0.1, nu_low = 0.06;
    double nu_max = 1.0 + ((nu_high-nu_low)/nu_inc); //number of nu loops to run from 0 to 1(add 1 to include nu=0)
  
    //*****REGULAR OUTPUT FILES*****
    //ofstream p_risk_out ("H://fourthyear/CAModel/Results/Prob_Risk.txt"); 
    //ofstream t_p_risk_out ("C:/Computing/MSci/CAmodel/Results/Theory_Prob_Risk.txt"); 
 
    //for (int n = 0; n < nu_max; n++)
    //{
		//*****INITIALISE NU LOOP VARIABLES***** 
        //double nu = nu_low;
        //nu = nu + (nu_inc * n);
        double nu = 0.1; //***ONLY IF TESTING OUTSIDE OF NU LOOP
        cout << "nu = " << nu << endl;
     
        //*******DECLARE CONSTANTS*******
        //unsigned long long seed[] = {29487593679,2342646334,63564373,235989523,49085295,38955829789,982751975,1985719575,23523556986,5893257998};  //initiaise random seed
        unsigned long long seed1 = 9348753948759124;
        init_genrand64(seed1);

		// int ecount = 0;             //initialise excited cells counter
		// int AFtimecount = 0;
        double delta = 0.05;        //prob of defective cells
        double epsilon = 0.05;      //prob of cell not exciting
        double fib_thresh = L*1.2;  //threshold of number of excited cells to define 'in AF' state
        double theory_p_risk = 1 - pow( (1 - pow( (1 - nu), refp)), delta*L*L); //theoretical prob risk
		//int excitedcell[tmax] = {0};  //vector to store number of excited cells per time
         
        //*********ARRAYS TO STORE DEFECTIVE AND VERTICALLY COUPLED CELLS*********
        int def_cells[L][L] = {0}, ycouples[L][L] = {0};
 
        //***********INITIALISE OUTPUT FILES***********
        //stringstream string1;  
        ////string1 << "C:/Computing/MSci/CAmodel/Results/EXCITE_size_" << L << "_nu_"<< nu << ".txt";
        //string data = string1.str();
        //ofstream threshold(data);
 
        //******VISUALISATION VARIABLES******
		int width = 1000;
		int height = 1000;
        double sizecell = double(width)/double(L);
        double greyshadeinc = floor(255/refp);
 
		/*string visualisation;
		cout << "Visualise? Y or N" <<endl;
		cin >> visualisation;*/
		
		//create the window
		sf::RenderWindow window(sf::VideoMode (width,height), "CAModel");
		
		//create cell(s) of size 10x10
		sf::RectangleShape cellrest(sf::Vector2f(sizecell, sizecell));
		cellrest.setFillColor(sf::Color(0,0,0));
 
		sf::RectangleShape cellexcite(sf::Vector2f(sizecell, sizecell));
		cellexcite.setFillColor(sf::Color(255,0,0));
 
		sf::RectangleShape cellrefract(sf::Vector2f(sizecell, sizecell));
 
        ////**********************SEED LOOP**********************************
        //for (int s = 0; s < sizeof(seed)/sizeof(*seed); s++)
        //{
          //init_genrand64(seed[s]);
 
            //**********INITIALISE LATTICE**********
            int originalLattice[L][L] = {0};
            int nextLattice[L][L] = {0};
 
            //*****RANDOMLY CREATE DEFECTIVE AND VERTICALLY COUPLED CELLS*****
            for (int i = 0; i < L; i++) 
            {
                for (int j = 1; j < L; j++) 
                {
                    //DEFECTIVE
                    if(genrand64_real2() < delta) 
                        def_cells[i][j] = 1;
                    else
                        def_cells[i][j] = 0;
                         
                    //VERTICAL COUPLING
                    if(genrand64_real2() < nu) ycouples[i][j] = 1;
                    else ycouples[i][j] = 0;
                }
            }
 
        ////*******PRINT OUT COUPLED CELL VECTOR********
        //for(int i=0; i < L; i++) 
        //{
		//	for (int j=0; j < L; j++)
        //  cout << ycouples[i][j] << " "; 
        //  cout << endl;                //print out grid layout
        //}
 
        ////*******PRINT OUT DEFECTIVE CELL VECTOR********
        //for(int i=0; i < L; i++) 
        //{
		//	for (int j=0; j < L; j++)
        //  cout << def_cells[i][j] << " "; 
        //  cout << endl;                //print out grid layout
        //       
        //}
 
        //******VISUALISATION STUFF******
        while (window.isOpen())
        {
			//handle events
			
				sf::Event event;
				while (window.pollEvent(event))
				{
					if(event.type == sf::Event::Closed)
					window.close();
				}
			//*********************************
 
            //*********EXCITE PACEMAKER CELLS************
            for (int i = 0; i < L; i++) originalLattice[i][0] = 1;
 
            //******************TIME LOOP******************
            for (int t = 0; t < tmax; t++)
            {
                window.clear();
                //**********LATTICE CELL LOOP**********
                for (int i = 0; i < L; i++) 
                {
                    for (int j = 1; j < L; j++) 
                    {
                        //***********VERTICAL EXCITATION***********
                        //if (FindCell(ycouples, i, j) == true)
                        if(ycouples[i][j] == 1)
                        {
                            //****BOUNDARY CONDITION****
                            if (i == L-1)
                            {
                                //if cell in last row is excited, excite cell in first row, if not defective
                                if(originalLattice[i][j] == 1 && originalLattice[0][j] == 0)
                                {
                                    //excite first row if not defective 
                                    if(def_cells[0][j] == 1 && (genrand64_real2() < epsilon)) nextLattice[0][j] = 0;
                                    else nextLattice[0][j] = 1;
                                }
                                //if cell in first row is excited, excite cell in last row in next timestep, if not defective
                                else if(originalLattice[i][j] == 0 && originalLattice[0][j] == 1)
                                {
                                    if(def_cells[i][j] == 1 && (genrand64_real2() < epsilon)) nextLattice[i][j] = 0;
                                    else nextLattice[i][j] = 1;
                                }
                            }
                             
                            else
                            {
                                //if vertical neighbour is excited, excite cell in next timestep, if not defective
                                if(originalLattice[i][j] == 0 && originalLattice[i+1][j] == 1)
                                {
                                    if(def_cells[i][j] == 1 && (genrand64_real2() < epsilon)) nextLattice[i][j] = 0;
                                    else nextLattice[i][j] = 1;
                                }
                                //if cell is excited, excite vertical neighbour in next timestep, if not defective
                                else if(originalLattice[i][j] == 1 && originalLattice[i+1][j] == 0)
                                {
                                    if(def_cells[i+1][j] == 1 && (genrand64_real2() < epsilon)) nextLattice[i+1][j] = 0;
 
                                    else nextLattice[i+1][j] = 1;
                                }
                            }
                        }
                         
                        //***************HORIZONTAL CELLS****************
                        //****CHECK LEFT NEIGHBOURS****
                        if (originalLattice[i][j - 1] == 1 && originalLattice[i][j]==0)
                        {
                            if(def_cells[i][j] == 1 && (genrand64_real2() < epsilon)) nextLattice[i][j] = 0;
 
                            else nextLattice[i][j] = 1;
                        } 
                        //****CHECK RIGHT NEIGHBOURS****
                        else if (originalLattice[i][j + 1] == 1 && originalLattice[i][j] == 0 && j != L-1)
                        {
                            if(def_cells[i][j] == 1 && (genrand64_real2() < epsilon)) nextLattice[i][j] = 0;
                            else nextLattice[i][j] = 1;
                        }
                        //***************UPDATE REFRACTORY CELL STATE***************
                        //cells that have been excited in previous time step, start refractory period
                        if (originalLattice[i][j] == 1) nextLattice[i][j] = (-1)*refp;
                        //increment cells that are already refractory by 1, ignore if the cell is in the resting state "0"
                        else if (originalLattice[i][j] < 0) 
                        {
                            nextLattice[i][j] = originalLattice[i][j] + 1;
                            /* if( visualisation =="Y")
							{*/
								double greyshade = abs(greyshadeinc*nextLattice[i][j]);
								cellrefract.setFillColor(sf::Color(greyshade,greyshade, greyshade));
								cellrefract.setPosition(sizecell*j,sizecell*i);
								window.draw(cellrefract);
						/*	}*/
                        }
                         
                        //****************DRAW CELLS********************
						/*if( visualisation =="Y")
						{*/
                            if (originalLattice[i][j]==1)
                            {
                                cellexcite.setPosition(sizecell*j,sizecell*i);
                                window.draw(cellexcite);
                            }
                         
                            if (originalLattice[i][j]==0)
                            {
                                cellrest.setPosition(sizecell*j,sizecell*i);
                                window.draw(cellrest);
                            }   
						/*}*/
                    } //close j-loop  
                     
                    //***************UPDATE PACEMAKER CELL STATE**************
                    //if pacemaker cell is excited, make refractory in the next time step
                    if (originalLattice[i][0] == 1) nextLattice[i][0] = (-1)*SAP;
                    //increment refractory states by 1 (if not already in the last refractory state "-1"
                    else if (originalLattice[i][0] != 1 && originalLattice[i][0] != -1) nextLattice[i][0] = originalLattice[i][0] + 1;
                    //if pacemaker cells are in the last refractory state, excite
                    else if (originalLattice[i][0] == -1) nextLattice[i][0] = 1;
                     
                } //close i-loop
             
                //*****CHECK IF SYSTEM IS FIBRILLATORY*****
 
                //store and clear excited cell counter
                //excitedcell[t] += ecount;

                //******************OUTPUT MODEL*******************
            /*  cout << "time = " << t << " ";
                cout << endl;
                for (int i = 0; i < L; i++)
                {
                    for (int j = 0; j < L; j++)
                        cout << originalLattice[i][j] << " "; 
                        cout << endl;               
                }*/             
 
                //***********UPDATE TIMESTEP***********
 
				for(int i=0; i<L; i++)
                {
                    for(int j=0; j<L; j++)
                    {
						//if(nextLattice[i][j] == 1) ecount++;
						originalLattice[i][j] = nextLattice[i][j];
						nextLattice[i][j] = 0;
                    }
                }

				//if (ecount > (fib_thresh)) AFtimecount++;

				//ecount = 0;
                //***************PROGRESS BAR****************
                //if(t%(tmax/10) ==0) cout << double(t)/double(tmax)*100 << "%" <<endl;
                //if(t==tmax-1) cout << "seed " << s+1 << " completed" << endl;
 
 
                //***OUTPUT VISUALISATION***********
               /* if (visualisation =="Y")*/ window.display();
 
            }//close time
 
        }//close while loop
		
		//if (AFtimecount>0)cout << AFtimecount << endl;
	
	//} //close seed-loop
	
	//*****OUTPUT TO FILE AVERAGE EXCITED CELL PER TIME********
    //for (int i=0;i<tmax;i++)
    //{
	//	threshold << i << '\t'<< excitedcell[i]/(sizeof(seed)/sizeof(*seed)) << endl;
    //}

	//*****OUTPUT TO FILE PROB RISK********
	//double avgAFtime = double (AFtimecount/(sizeof(seed)/sizeof(*seed)));
	//double p_risk = avgAFtime/double(tmax);
	//cout << "p risk" << '\t' << p_risk << endl;
	//p_risk_out << nu << '\t' << p_risk << endl;
	//cout << " " << endl;

//}//close nu loop
 
} //close main-loop