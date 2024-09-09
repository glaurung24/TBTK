/* Copyright 2017 Kristofer Björnson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file ManyParticleContext.cpp
 *
 *  @author Andreas Theiler
 *  @author Kristofer Björnson
 */

#include "TBTK/LadderOperator.h"

using namespace std;

namespace TBTK{

LadderOperator::LadderOperator(){
}

LadderOperator::LadderOperator(
	Type type,
	Statistics statistics,
	const HoppingAmplitudeSet *hoppingAmplitudeSet,
	unsigned int state,
	unsigned int numBitsPerState,
	unsigned int maxOccupation,
	const FockState &templateState,
	const BitRegister &fermionMask
) :
	stateMask(templateState.bitRegister),
	leastSignificantBit(templateState.bitRegister),
	maxOccupation(templateState.bitRegister),
	moreSignificantFermionMask(fermionMask)
{
    TBTKExit(
        "LadderOperator::LadderOperator(...)",
    "Function not implemented yet",
    "Do the implementation..."
    );
	// this->type = type;
	// this->statistics = statistics;
	// this->hoppingAmplitudeSet = hoppingAmplitudeSet;
	// this->state = state;

	// leastSignificantBitIndex = state*numBitsPerState;

	// for(unsigned int n = 0; n < stateMask.size(); n++){
	// 	if(n >= leastSignificantBitIndex && n < leastSignificantBitIndex + numBitsPerState)
	// 		stateMask.set(n, 1);
	// 	else
	// 		stateMask.set(n, 0);

	// 	if(n == leastSignificantBitIndex)
	// 		leastSignificantBit.set(n, 1);
	// 	else
	// 		leastSignificantBit.set(n, 0);
	// }

	// this->maxOccupation = maxOccupation;
	// this->maxOccupation = (this->maxOccupation << leastSignificantBitIndex); //TODO


	// for(unsigned int n = 0; n < moreSignificantFermionMask.size(); n++){
	// 	this->moreSignificantFermionMask.set(n, false);
	// 	if(leastSignificantBit[n])
	// 		break;
	// }
}


LadderOperator::~LadderOperator(){
}


typename LadderOperator::Type LadderOperator::getType() const{
	return type;
}


const Index LadderOperator::getPhysicalIndex() const{
	return hoppingAmplitudeSet->getPhysicalIndex(state);
}


unsigned int LadderOperator::getState() const{
	return state;
}


unsigned int LadderOperator::getNumParticles(
	const FockState &fockState
) const{
	return ((fockState.getBitRegister() & stateMask) >> leastSignificantBitIndex).to_ulong();
}


FockState& LadderOperator::operator*(
	FockState &rhs
) const{
    TBTKExit(
        "LadderOperator::operator*(...)",
    "Function not implemented yet",
    "Do the implementation..."
    );
	// switch(type){
	// case Type::Creation:
	// 	if((rhs.bitRegister & stateMask) == maxOccupation){
	// 		rhs.bitRegister.setMostSignificantBit();
	// 		break;
	// 	}
	// 	rhs.bitRegister += leastSignificantBit;
	// 	break;
	// case Type::Annihilation:
	// 	if(!(rhs.bitRegister & stateMask).any()){
	// 		rhs.bitRegister.setMostSignificantBit();
	// 		break;
	// 	}
	// 	rhs.bitRegister -= leastSignificantBit;
	// 	break;
	// default:
	// 	TBTKExit(
	// 		"LadderOperator::operator*()",
	// 		"This should never happen.",
	// 		"Contact the developer."
	// 	);
	// }

	// switch(statistics){
	// case Statistics::FermiDirac:
	// 	rhs.prefactor *= pow(-1, (rhs.bitRegister & moreSignificantFermionMask).count());
	// 	break;
	// case Statistics::BoseEinstein:
	// 	break;
	// default:
	// 	TBTKExit(
	// 		"LadderOperator::operator*()",
	// 		"This should never happen.",
	// 		"Contact the developer."
	// 	);
	// }

	return rhs;
}

};	//End of namespace TBTK