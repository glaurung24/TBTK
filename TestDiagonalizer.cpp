#include "TBTK/TBTK.h"
#include "TBTK/Solver/Diagonalizer.h"
#include "TBTK/PropertyExtractor/Diagonalizer.h"
#include <iostream>


#include <complex>

using namespace std;
using namespace std::chrono;
using namespace TBTK;

const double EPS=1E-4;

int main(int argc, char **argv){
	//Initialize TBTK.
	TBTK::Initialize();
	   //Parameters.
    const int SIZE_X = 50;
    const int SIZE_Y = 50;
    const double t = -1;
    const double mu = 0.0001;
    //Set up the Model.
    TBTK::Model model;
    for(int x = 0; x < SIZE_X; x++){
        for(int y = 0; y < SIZE_Y; y++){
            model << TBTK::HoppingAmplitude(
                mu,
                {x,y},
                {x,y}
            );
            if(x+1 < SIZE_X){
                model << TBTK::HoppingAmplitude(
                    t,
                    {x+1, y},
                    {x, y}
                ) + TBTK::HC;
            }
            if(y+1 < SIZE_Y){
                model << TBTK::HoppingAmplitude(
                    t,
                    {x, y+1},
                    {x, y}
                ) + TBTK::HC;
            }
        }
    }
    model.construct();
    //Set up the Solver.

    Index index = {1,1};

    TBTK::Solver::Diagonalizer solverGPU;
    model.setTemperature(0.);
    solverGPU.setModel(model);
	solverGPU.setUseGPUAcceleration(true);
    solverGPU.run();
    PropertyExtractor::Diagonalizer peGPU(solverGPU);
    if((real(peGPU.calculateExpectationValue(index, index)) -0.5) < EPS){
        return 0;
    }
	else{
        return -1;
    }
}