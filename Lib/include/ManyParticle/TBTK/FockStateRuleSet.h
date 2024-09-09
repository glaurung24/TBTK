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
 *  @file FockStateRuleSet.h
 *  @brief FockStateRuleSet.
 *
 *  @author Kristofer Björnson
 *  @author Andreas Theiler
 */

#ifndef COM_DAFER45_TBTK_FOCK_STATE_RULE_SET
#define COM_DAFER45_TBTK_FOCK_STATE_RULE_SET

#include "TBTK/FockStateRule/FockStateRule.h"
#include "TBTK/FockSpace.h"

#include <vector>
#include <memory>

namespace TBTK{
class FockStateRuleSet{
public:
	/** Constructor */
	FockStateRuleSet();

	/** Destructor. */
	~FockStateRuleSet();

	/** Check whether a given FockState fullfills the rule with respect to
	 *  a particular FockSpace. */
	bool isSatisfied(
		const FockSpace &fockSpace,
		const FockState &fockState
	) const;

	/** Add FockStateRule. */
	void addFockStateRule(const std::shared_ptr<FockStateRule::FockStateRule> fockStateRule);

	/** Get size. */
	unsigned int getSize() const;

	/** Comparison operator. */
	bool operator==(const FockStateRuleSet &rhs) const;

	/** Multiplication operator between a LadderOperator and a
	 *  FockStateRuleSet. Creates a new FockStateRuleSet containing the
	 *  FockStateRules that results from applying the operator to each
	 *  FockStateRule in the current FockStateRuleSet. */
	// friend FockStateRuleSet operator*(
	// 	const LadderOperator &ladderOperator,
	// 	const FockStateRuleSet &fockStateRuleSet
	// );

	/** Print FockStateRuleSet. */
	void print() const;
private:
	/** FockStateRules. */
	std::vector<std::shared_ptr<FockStateRule::FockStateRule>> fockStateRules;
};

inline void FockStateRuleSet::addFockStateRule(
	const std::shared_ptr<FockStateRule::FockStateRule> fockStateRule
){
	for(unsigned int n = 0; n < fockStateRules.size(); ++n)
		if(*(fockStateRules.at(n).get()) == *(fockStateRule.get()))
			return;
	fockStateRules.push_back(fockStateRule);
}

inline unsigned int FockStateRuleSet::getSize() const{
	return fockStateRules.size();
}

// inline FockStateRuleSet operator*(
// 	const LadderOperator &ladderOperator,
// 	const FockStateRuleSet &fockStateRuleSet
// ){
// 	FockStateRuleSet newRuleSet;
// 	for(unsigned int n = 0; n < fockStateRuleSet.fockStateRules.size(); n++)
// 		newRuleSet.addFockStateRule(ladderOperator*fockStateRuleSet.fockStateRules.at(n));

// 	return newRuleSet;
// }

inline void FockStateRuleSet::print() const{
	Streams::out << "FockStateRuleSet{\n";
	for(unsigned int n = 0; n < fockStateRules.size(); n++){
		if(n > 0)
			Streams::out << ",\n";
		Streams::out << "\t";
		fockStateRules.at(n)->print();
	}
	Streams::out << "\n}\n";
}

};	//End of namespace TBTK

#endif
/// @endcond
