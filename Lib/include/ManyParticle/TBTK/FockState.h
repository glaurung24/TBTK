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
 *  @file FockState.h
 *  @brief FockState.
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_FOCK_STATE
#define COM_DAFER45_TBTK_FOCK_STATE

#include "TBTK/Streams.h"
#include "TBTK/BitRegister.h"

namespace TBTK{

class FockState{
public:
	/** Constructor. */
	FockState(unsigned int exponentialDimension);

	/** Copy constructor. */
	FockState(const FockState &fockState);

	/** Destructor. */
	~FockState();

	/** Returns true if the vector is the state is the null vector. */
	bool isNull() const;

	/** Returns the BitRegister. */
	const BitRegister& getBitRegister() const;

	/** Returns the BitRegister. */
	BitRegister& getBitRegister();

	/** Get prefactor. */
	int getPrefactor() const;

	/** Get number of particles. */
//	unsigned int getNumFermions() const;

	/** Print. */
	void print() const;
private:
	/** Allow the FockSpace to immediatly access the internal storage. */
	friend class FockSpace;

	/** Allow operators to operate immediately on the internal storage. */
	friend class LadderOperator;

	/** Bit register used to store occupation numbers. */
	BitRegister bitRegister;

	/** Prefactor containing the sign and amplitude of the state a|psi>.
	 *  For efficiency sign(a)a^2 is stored rather than a. */
	int prefactor;
};

FockState::FockState(unsigned int exponentialDimension
) :
	bitRegister(exponentialDimension+1)
{
	bitRegister.clear();
	prefactor = 1;
}

FockState::FockState(const FockState &fockState
) :
	bitRegister(fockState.bitRegister)
{
	prefactor = fockState.prefactor;
}


FockState::~FockState(){
}


bool FockState::isNull() const{
	return bitRegister.getMostSignificantBit();
}


const BitRegister& FockState::getBitRegister() const{
	return bitRegister;
}


BitRegister& FockState::getBitRegister(){
	return bitRegister;
}


int FockState::getPrefactor() const{
	return prefactor;
}

/*
unsigned int FockState::getNumFermions() const{
	return bitRegister.getNumOneBits();
}*/


void FockState::print() const{
	Streams::out << prefactor << "|";
	for(int n = bitRegister.getNumBits()-1; n >= 0; n--){
		Streams::out << bitRegister[n];
		if(n%8 == 0 && n != 0)
			Streams::out << " ";
	}
	Streams::out << ">\n";
}

};	//End of namespace TBTK

#endif
/// @endcond
