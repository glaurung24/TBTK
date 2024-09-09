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
		const FockState &templateState,
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
		const FockState &fockState
	) const;

	/** Multiplication operator. */
	FockState& operator*(FockState &rhs) const;
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

};	//End of namespace TBTK

#endif
/// @endcond
