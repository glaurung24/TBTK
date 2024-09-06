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

/// @cond TBTK_FULL_DOCUMENTATION
/** @package TBTKcalc
 *  @file LookupTableMap.h
 *  @brief LookupTableMap.
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_LOOKUP_TABLE_MAP
#define COM_DAFER45_TBTK_LOOKUP_TABLE_MAP

#include "TBTK/FockStateMap/FockStateMap.h"
#include "TBTK/BitRegister.h"
#include "TBTK/ExtensiveBitRegister.h"

namespace TBTK{
namespace FockStateMap{

class LookupTableMap : public FockStateMap{
public:
	/** Constructor. */
	LookupTableMap(unsigned int exponentialDimension);

	/** Destructor. */
	virtual ~LookupTableMap();

	/** Get many-body Hilbert space size. */
	virtual unsigned int getBasisSize() const;

	/** Get many-body Hilbert space index for corresponding FockState. */
	virtual unsigned int getBasisIndex(
		const FockState &fockState
	) const;

	/** Get FockState for corresponding many-body Hilbert space index. */
	virtual FockState getFockState(unsigned int index) const;

	/** Add state. */
	void addState(const FockState &fockState);
private:
	/** List of FockStates. */
	std::vector<FockState> states;
};


LookupTableMap::LookupTableMap(
	unsigned int exponentialDimension
) :
	FockStateMap(exponentialDimension)
{
}


LookupTableMap::~LookupTableMap(){
}


unsigned int LookupTableMap::getBasisSize() const{
	return states.size();
}

unsigned int LookupTableMap::getBasisIndex(
	const FockState &fockState
) const{
	unsigned int min = 0;
	unsigned int max = states.size()-1;
	while(min <= max){
		unsigned int currentState = (min+max)/2;
		if(fockState.getBitRegister() > states.at(currentState).getBitRegister())
			min = currentState + 1;
		else if(fockState.getBitRegister() < states.at(currentState).getBitRegister())
			max = currentState - 1;
		else if(fockState.getBitRegister() == states.at(currentState).getBitRegister())
			return currentState;
	}
	TBTKExit(
		"LookupTableFockStateMap::getBasisIndex()",
		"FockState not found.",
		""
	);
}


FockState LookupTableMap::getFockState(
	unsigned int index
) const{
	return states.at(index);
}


void LookupTableMap::addState(
	const FockState &fockState
){
	if(
		states.size() == 0
		|| states.back().getBitRegister() < fockState.getBitRegister()
	){
		states.push_back(fockState);
	}
	else{
		unsigned int min = 0;
		unsigned int max = states.size()-1;
		while(min <= max){
			unsigned int currentState = (min+max)/2;
			if(
				fockState.getBitRegister()
				> states.at(currentState).getBitRegister()
			){
				min = currentState + 1;
			}
			else if(
				fockState.getBitRegister()
				< states.at(currentState).getBitRegister()
			){
				max = currentState - 1;
			}

			if(min >= max){
				if(
					fockState.getBitRegister()
						< states.at(
							currentState
						).getBitRegister()
				){
					states.insert(
						states.begin() + currentState,
						fockState
					);
				}
				else{
					states.insert(
						states.begin() + currentState
							+ 1,
						fockState
					);
				}
				break;
			}
		}
	}
}

};	//End of namespace FockStateMap
};	//End of namespace TBTK

#endif
/// @endcond
