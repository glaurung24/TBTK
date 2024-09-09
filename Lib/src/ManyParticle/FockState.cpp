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

/** @file FockState.cpp
 *
 *  @author Kristofer Björnson
 *  @author Andreas Theiler
 */


#include "TBTK/FockState.h"

namespace TBTK{

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
	for(int n = bitRegister.size()-1; n >= 0; n--){
		Streams::out << bitRegister[n];
		if(n%8 == 0 && n != 0)
			Streams::out << " ";
	}
	Streams::out << ">\n";
}



};	//End of namespace TBTK