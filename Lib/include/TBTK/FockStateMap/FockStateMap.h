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
 *  @file FockStateMap.h
 *  @brief FockStateMap.
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_FOCK_STATE_MAP
#define COM_DAFER45_TBTK_FOCK_STATE_MAP

#include "TBTK/FockState.h"

namespace TBTK{
namespace FockStateMap{

class FockStateMap{
public:
	/** Constructor. */
	FockStateMap(unsigned int exponentialDimension);

	/** Destructor. */
	virtual ~FockStateMap();

	/** Get many-body Hilbert space size. */
	virtual unsigned int getBasisSize() const = 0;

	/** Get many-body Hilbert space index for corresponding FockState. */
	virtual unsigned int getBasisIndex(
		const FockState &fockState
	) const = 0;

	/** Get FockState for corresponding many-body Hilbert space index. */
	virtual FockState getFockState(
		unsigned int index
	) const = 0;

	/** Get exponential dimension. */
	unsigned int getExponentialDimension() const;
private:
	unsigned int exponentialDimension;
};


inline FockStateMap::FockStateMap(unsigned int exponentialDimension){
	this->exponentialDimension = exponentialDimension;
}


inline FockStateMap::~FockStateMap(){
}


inline unsigned int FockStateMap::getExponentialDimension() const{
	return exponentialDimension;
}

};	//End of namespace FockStateMap
};	//End of namespace TBTK

#endif
/// @endcond
