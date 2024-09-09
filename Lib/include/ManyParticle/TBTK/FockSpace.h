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
 *  @file FockSpcae.h
 *  @brief Fock space.
 *
 *  @author Kristofer Björnson
 */


#ifndef COM_DAFER45_TBTK_FOCK_SPACE
#define COM_DAFER45_TBTK_FOCK_SPACE

#include "TBTK/BitRegister.h"
#include "TBTK/FockState.h"
#include "TBTK/FockStateMap/DefaultMap.h"
#include "TBTK/FockStateMap/FockStateMap.h"
#include "TBTK/FockStateMap/LookupTableMap.h"
// #include "TBTK/FockStateRuleSet.h"
// #include "TBTK/FockStateRule/FockStateRule.h"
// #include "TBTK/FockStateRule/WrapperRule.h"
#include "TBTK/HoppingAmplitudeSet.h"
#include "TBTK/LadderOperator.h"
//#include "Model.h"
#include "TBTK/Statistics.h"

namespace TBTK{

class FockSpace{
public:
	/** Constructor. */
	FockSpace();

	/** Constructor. */
	FockSpace(
		const HoppingAmplitudeSet *hoppingAmplitudeSet,
		Statistics statistics,
		unsigned int maxParticlesPerState
	);

	/** Copy constructor. */
	FockSpace(const FockSpace &fockSpace);

	/** Destructor. */
	~FockSpace();

	/** Assignment operator. */
	FockSpace& operator=(const FockSpace &rhs);

	/** Get operators. */
	LadderOperator const* const* getOperators() const;

	/** Get the vacuum state. */
	FockState getVacuumState() const;

	/** Returns the number of fermions in the state. */
	unsigned int getNumFermions(
		const FockState &fockState
	) const;

	/** Get number of particles in the single particle state with given
	 *  index for the given FockState. */
	unsigned int getNumParticles(
		const FockState &fockState,
		const Index &index
	) const;

	/** Get number of particles in the single particle states that
	 *  satisfies the given index pattern for the given FockState. */
	unsigned int getSumParticles(
		const FockState &fockState,
		const Index &pattern
	) const;

	/** Create FockStateMap. */
	FockStateMap::FockStateMap* createFockStateMap(
		int numParticles
	) const;

	/** Create FockStateMap. */
	// FockStateMap::FockStateMap* createFockStateMap(
	// 	const FockStateRule::FockStateRule &rule
	// ) const;

	// /** Create FockStateMap. */
	// FockStateMap::FockStateMap* createFockStateMap(
	// 	std::initializer_list<const FockStateRule::WrapperRule> rules
	// ) const;

	// /** Create FockStateMap. */
	// FockStateMap::FockStateMap* createFockStateMap(
	// 	std::vector<FockStateRule::WrapperRule> rules
	// ) const;

	// /** Create FockStateMap. */
	// FockStateMap::FockStateMap* createFockStateMap(
	// 	const FockStateRuleSet &rules
	// ) const;

	/** Get amplitude set. */
	const HoppingAmplitudeSet* getHoppingAmplitudeSet() const;
private:
	/** Statistics. */
	Statistics statistics;

	/** Number of bits needed to encode all states. */
	unsigned int exponentialDimension;

	/** HoppingAmplitudeSet holding the single particle representation. */
	const HoppingAmplitudeSet *hoppingAmplitudeSet;

	/** Vacuum state used as template when creating new states. */
	FockState *vacuumState;

	/** Operators. */
	LadderOperator **operators;

	/** Converts a FockState to a many-body Hilbert space index. */
	unsigned int (*stateMapCallback)(
		const FockState &fockState
	);

	/** Fock state map for mapping FockStates to many-body Hilbert space
	 *  indices, and vice versa. */
	FockStateMap::FockStateMap *fockStateMap;
};


};	//End of namespace TBTK

#endif
/// @endcond
