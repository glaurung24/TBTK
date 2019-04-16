/* Copyright 2019 Kristofer Björnson
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

/** @package TBTKcalc
 *  @file GaussianBasisGenerator.h
 *  @brief Generates a basis of @link GaussianState GaussianStates @endlink.
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_GAUSSIAN_BASIS_GENERATOR
#define COM_DAFER45_TBTK_GAUSSIAN_BASIS_GENERATOR

#include "TBTK/GaussianState.h"

#include <vector>

namespace TBTK{

class GaussianBasisGenerator{
public:
	/** Generate a basis of @link GaussianState GaussianStates@endling. */
	std::vector<GaussianState> generateBasis() const;
private:
};

};	//End of namespace TBTK

#endif
