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

/** @file TBTK.cpp
 *  @author Kristofer Björnson
 */

#include "TBTK/Quantity/Constants.h"
#include "TBTK/Quantity/Derived.h"
#include "TBTK/Quantity/Quantity.h"
#include "TBTK/TBTK.h"
#include "TBTK/UnitHandler.h"

namespace TBTK{

void Initialize(){
	Quantity::Constants::initialize();
	Quantity::initializeBaseQuantities();
	Quantity::initializeDerivedQuantities();
	UnitHandler::initialize();
}

};
