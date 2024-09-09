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
 *  @file ManyParticleContext.h
 *  @brief Many-particle context.
 *
 *  @author Kristofer Björnson
 */


#ifndef COM_DAFER45_TBTK_MANY_PARTICLE_CONTEXT
#define COM_DAFER45_TBTK_MANY_PARTICLE_CONTEXT

#include "TBTK/BitRegister.h"
#include "TBTK/FockSpace.h"
// #include "TBTK/FockStateRuleSet.h"
#include "TBTK/InteractionAmplitudeSet.h"
#include "TBTK/SingleParticleContext.h"

#include <memory>

namespace TBTK{

class ManyParticleContext{
public:
	/** Constructor. */
	ManyParticleContext();

	/** Constructor. The ManyParticleContext assumes ownership of the
	 *  FockSpace and will destroy it at destrucion. */
//	ManyParticleContext(FockSpace *fockSpace);
	ManyParticleContext(
		const SingleParticleContext *singleParticleContext
	);

	/** Constructor. The ManyParticleContext assumes ownership of the
	 *  FockSpace and will destroy it at destrucion. */
//	ManyParticleContext(FockSpace<ExtensiveBitRegister> *fockSpace);

	/** Destructor. */
	~ManyParticleContext();



	/** Returns a pointer to the FockState contained by the
	 *  wrapper. */
	FockSpace* getFockSpaceBitRegister();

	/** Add rule that restricts the Fock space. */
	void addFockStateRule(const std::shared_ptr<FockStateRule::FockStateRule> rule);

	/** Add InteractionAmplitude. */
	void addIA(InteractionAmplitude ia);

	/** Get InteractionAmplitudeSet. */
	const InteractionAmplitudeSet* getInteractionAmplitudeSet() const;

	/** Get FockStateRules. */
	const FockStateRuleSet& getFockStateRuleSet() const;
private:
	/** Pointer to FockSpace using BitRegsiter. */
	std::shared_ptr<FockSpace> fockSpace;

	/** Rules specifying the relevant subspace. */
	FockStateRuleSet fockStateRuleSet;

	/** Interaction amplitude set. */
	std::shared_ptr<InteractionAmplitudeSet> interactionAmplitudeSet;
};


inline FockSpace* ManyParticleContext::getFockSpaceBitRegister(){
	return fockSpace.get();
}


inline void ManyParticleContext::addFockStateRule(const std::shared_ptr<FockStateRule::FockStateRule> rule){
	fockStateRuleSet.addFockStateRule(rule);
}

inline void ManyParticleContext::addIA(InteractionAmplitude ia){
	interactionAmplitudeSet.get()->addIA(ia);
}

inline const InteractionAmplitudeSet* ManyParticleContext::getInteractionAmplitudeSet() const{
	return interactionAmplitudeSet.get();
}

inline const FockStateRuleSet& ManyParticleContext::getFockStateRuleSet() const{
	return fockStateRuleSet;
}

};	//End of namespace TBTK

#endif
/// @endcond
