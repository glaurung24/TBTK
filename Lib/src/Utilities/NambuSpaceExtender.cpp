/* Copyright 2018 Kristofer Björnson and Andreas Theiler
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

/** @file NambuSpaceExtender.cpp
 *
 *  @author Kristofer Björnson
 */

#include "TBTK/NambuSpaceExtender.h"

using namespace std;

namespace TBTK{

Model NambuSpaceExtender::extend(const Model &model){
	const HoppingAmplitudeSet &hoppingAmplitudeSet
		= model.getHoppingAmplitudeSet();

	Model newModel;
	for(
		HoppingAmplitudeSet::ConstIterator iterator
			= hoppingAmplitudeSet.cbegin();
		iterator != hoppingAmplitudeSet.cend();
		++iterator
	){
		Index toIndex0 = (*iterator).getToIndex();
		Index fromIndex0 = (*iterator).getFromIndex();

		Index toIndex1 = fromIndex0;
		Index fromIndex1 = toIndex0;

		toIndex0.push_back(0);
		fromIndex0.push_back(0);

		toIndex1.push_back(1);
		fromIndex1.push_back(1);

		if((*iterator).getIsCallbackDependent()){
			newModel << HoppingAmplitude(
				(*iterator).getAmplitudeCallback(),
				toIndex0,
				fromIndex0
			);
			newModel << HoppingAmplitude(
				(*iterator).getAmplitudeCallback(),
				toIndex1,
				fromIndex1
			);
		}
		else{
			newModel << HoppingAmplitude(
				(*iterator).getAmplitude(),
				toIndex0,
				fromIndex0
			);
			newModel << HoppingAmplitude(
				-(*iterator).getAmplitude(),
				toIndex1,
				fromIndex1
			);
		}
	}

	if(model.getChemicalPotential() != 0){
		for(int n = 0; n < model.getBasisSize(); n++){
			Index index0 = model.getHoppingAmplitudeSet(
			).getPhysicalIndex(n);

			Index index1 = index0;

			index0.push_back(0);
			index1.push_back(1);

			newModel << HoppingAmplitude(
				-model.getChemicalPotential(),
				index0,
				index0
			);
			newModel << HoppingAmplitude(
				model.getChemicalPotential(),
				index1,
				index1
			);
		}
	}
	newModel.setChemicalPotential(0);

	newModel.setTemperature(model.getTemperature());
	newModel.setStatistics(model.getStatistics());

	return newModel;
}

};	//End of namespace TBTK
