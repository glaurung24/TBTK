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
 *  @file SquareLattice.h
 *  @brief Square lattice model.
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_MODELS_SQUARE_LATTICE
#define COM_DAFER45_TBTK_MODELS_SQUARE_LATTICE

#include "TBTK/Model.h"

#include <complex>
#include <vector>

namespace TBTK{
namespace Models{

/** @brief Square lattice model. */
class SquareLattice : public Model{
public:
	/** Constructs a square lattice.
	 *
	 *  @param size The size of the lattice. Must have two components.
	 *  @param parameters The Model parameters. Must have two components,
	 *  where the first component is the on-site term, and the second is
	 *  the nearest neighbor hopping amplitude. */
	SquareLattice(
		const std::vector<unsigned int> &size,
		const std::vector<std::complex<double>> &parameters
	);
};

};	//End of namespace Models
};	//End of namespace TBTK

#endif