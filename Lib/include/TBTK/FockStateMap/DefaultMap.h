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
 *  @file DefaultMap.h
 *  @brief DefaultMap.
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_DEFAULT_MAP
#define COM_DAFER45_TBTK_DEFAULT_MAP

#include "TBTK/FockStateMap/FockStateMap.h"

namespace TBTK{
namespace FockStateMap{

class DefaultMap : public FockStateMap{
public:
	/** Constructor. */
	DefaultMap(unsigned int exponentialDimension);

	/** Destructor. */
	virtual ~DefaultMap();

	/** Get many-body Hilbert space size. */
	virtual unsigned int getBasisSize() const;

	/** Get many-body Hilbert space index for corresponding FockState. */
	virtual unsigned int getBasisIndex(const FockState &fockState) const;

	/** Get FockState for corresponding many-body Hilbert space index. */
	virtual FockState getFockState(unsigned int index) const;
private:
};


DefaultMap::DefaultMap(
	unsigned int exponentialDimension
) :
	FockStateMap(exponentialDimension)
{
}


DefaultMap::~DefaultMap(){
}


inline unsigned int DefaultMap::getBasisSize() const{
	return (1 << FockStateMap::getExponentialDimension());
}

};	//End of namespace FockStateMap
};	//End of namespace TBTK

#endif
/// @endcond
