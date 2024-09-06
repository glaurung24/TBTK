/* Copyright 2016 Kristofer Björnson
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

/// @cond TBTK_FULL_DOCUMENTATION
/** @package TBTKcalc
 *  @file LadderOperator.h
 *  @brief Ladder operator.
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_LADDER_OPERATOR
#define COM_DAFER45_TBTK_LADDER_OPERATOR

#include "TBTK/FockState.h"
#include "TBTK/HoppingAmplitudeSet.h"
#include "TBTK/Statistics.h"
#include "TBTK/BitRegister.h"

namespace TBTK{

class LadderOperator{
public:
	/** Operator type. */
	enum class Type {Creation, Annihilation};

	/** Constructor. */
	LadderOperator();

	/** Constructor. */
	LadderOperator(
		Type type,
		Statistics statistics,
		const HoppingAmplitudeSet *hoppingAmplitudeSet,
		unsigned int state,
		unsigned int numBitsPerState,
		unsigned int maxOccupation,
		const FockState<BitRegister> &templateState,
		const BitRegister &fermionMask
	);

	/** Destructor. */
	~LadderOperator();

	/** Get type. */
	Type getType() const;

	/** Get physical Index of associated single-particle state. */
	const Index getPhysicalIndex() const;

	/** Get associated single-particle state. */
	unsigned int getState() const;

	/** Get number of particles in the associated state. */
	unsigned int getNumParticles(
		const FockState<BitRegister> &fockState
	) const;

	/** Multiplication operator. */
	FockState<BitRegister>& operator*(FockState<BitRegister> &rhs) const;
private:
	/** Operator type. */
	Type type;

	/** Operator statistics. */
	Statistics statistics;

	/** Pointer to the corresponding HoppingAmplitudeSet. */
	const HoppingAmplitudeSet *hoppingAmplitudeSet;

	/** Single-particle state index. */
	unsigned int state;

	/** State mask. */
	BitRegister stateMask;

	/** Least significant bit. */
	BitRegister leastSignificantBit;

	/** Index of least significant bit. */
	unsigned int leastSignificantBitIndex;

	/** State corresponding to maximum number of occupied particles. */
	BitRegister maxOccupation;

	/** Mask for singeling out those fermions that have a higher bit index
	 *  than the state corresponding to this opperator. */
	BitRegister moreSignificantFermionMask;
};

template<typename BitRegister>
LadderOperator<BitRegister>::LadderOperator(){
}

template<typename BitRegister>
LadderOperator<BitRegister>::LadderOperator(
	Type type,
	Statistics statistics,
	const HoppingAmplitudeSet *hoppingAmplitudeSet,
	unsigned int state,
	unsigned int numBitsPerState,
	unsigned int maxOccupation,
	const FockState<BitRegister> &templateState,
	const BitRegister &fermionMask
) :
	stateMask(templateState.bitRegister),
	leastSignificantBit(templateState.bitRegister),
	maxOccupation(templateState.bitRegister),
	moreSignificantFermionMask(fermionMask)
{
	this->type = type;
	this->statistics = statistics;
	this->hoppingAmplitudeSet = hoppingAmplitudeSet;
	this->state = state;

	leastSignificantBitIndex = state*numBitsPerState;

	for(unsigned int n = 0; n < stateMask.getNumBits(); n++){
		if(n >= leastSignificantBitIndex && n < leastSignificantBitIndex + numBitsPerState)
			stateMask.set(n, 1);
		else
			stateMask.set(n, 0);

		if(n == leastSignificantBitIndex)
			leastSignificantBit.set(n, 1);
		else
			leastSignificantBit.set(n, 0);
	}

	this->maxOccupation = maxOccupation;
	this->maxOccupation = (this->maxOccupation << leastSignificantBitIndex);

	for(unsigned int n = 0; n < moreSignificantFermionMask.getNumBits(); n++){
		this->moreSignificantFermionMask.set(n, false);
		if(leastSignificantBit[n])
			break;
	}
}

template<typename BitRegister>
LadderOperator<BitRegister>::~LadderOperator(){
}

template<typename BitRegister>
typename LadderOperator<BitRegister>::Type LadderOperator<BitRegister>::getType() const{
	return type;
}

template<typename BitRegister>
const Index LadderOperator<BitRegister>::getPhysicalIndex() const{
	return hoppingAmplitudeSet->getPhysicalIndex(state);
}

template<typename BitRegister>
unsigned int LadderOperator<BitRegister>::getState() const{
	return state;
}

template<typename BitRegister>
unsigned int LadderOperator<BitRegister>::getNumParticles(
	const FockState<BitRegister> &fockState
) const{
	return ((fockState.getBitRegister() & stateMask) >> leastSignificantBitIndex).toUnsignedInt();
}

template<typename BitRegister>
FockState<BitRegister>& LadderOperator<BitRegister>::operator*(
	FockState<BitRegister> &rhs
) const{
	switch(type){
	case Type::Creation:
		if((rhs.bitRegister & stateMask) == maxOccupation){
			rhs.bitRegister.setMostSignificantBit();
			break;
		}
		rhs.bitRegister += leastSignificantBit;
		break;
	case Type::Annihilation:
		if(!(rhs.bitRegister & stateMask).toBool()){
			rhs.bitRegister.setMostSignificantBit();
			break;
		}
		rhs.bitRegister -= leastSignificantBit;
		break;
	default:
		TBTKExit(
			"LadderOperator<BitRegister>::operator*()",
			"This should never happen.",
			"Contact the developer."
		);
	}

	switch(statistics){
	case Statistics::FermiDirac:
		rhs.prefactor *= pow(-1, (rhs.bitRegister & moreSignificantFermionMask).getNumOneBits());
		break;
	case Statistics::BoseEinstein:
		break;
	default:
		TBTKExit(
			"LadderOperator<BitRegister>::operator*()",
			"This should never happen.",
			"Contact the developer."
		);
	}

	return rhs;
}

};	//End of namespace TBTK

#endif
/// @endcond
