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

/** @file BitRegister.cpp
 *
 *  @author Kristofer Björnson
 */


#include "TBTK/BitRegister.h"
#include "TBTK/Streams.h"

namespace TBTK{

BitRegister::BitRegister(unsigned long int numBits):
	boost::dynamic_bitset<>(numBits)
{
	MOST_SIGNIFICANT_BIT_MASK = boost::dynamic_bitset<>(numBits);
	MOST_SIGNIFICANT_BIT_MASK.set(this->size()-1, true);
}

BitRegister::BitRegister(unsigned long int numBits, unsigned long int value):
	boost::dynamic_bitset<>(numBits, value)
{
	MOST_SIGNIFICANT_BIT_MASK = boost::dynamic_bitset<>(numBits);
	MOST_SIGNIFICANT_BIT_MASK.set(MOST_SIGNIFICANT_BIT_MASK.size()-1, true);
}

BitRegister::BitRegister(const BitRegister &bitRegister) :
	boost::dynamic_bitset<>(bitRegister)
{
	MOST_SIGNIFICANT_BIT_MASK = boost::dynamic_bitset<>(this->size());
	MOST_SIGNIFICANT_BIT_MASK.set(MOST_SIGNIFICANT_BIT_MASK.size()-1, true);
}

BitRegister& BitRegister::operator=(const BitRegister &rhs){
	if(this != &rhs)
		*this = BitRegister(rhs);
	return *this;
}

BitRegister& BitRegister::operator=(unsigned long int rhs){
	reset();
	resize(sizeof(unsigned long int)*CHAR_BIT);
	BitRegister newRegister(sizeof(unsigned long int)*CHAR_BIT, rhs);
	*this |= newRegister; //TODO there should be a cleaner way...
	return *this;
}

BitRegister& BitRegister::operator=(unsigned int rhs){
	reset();
	resize(sizeof(unsigned int)*CHAR_BIT);
	BitRegister newRegister(sizeof(unsigned int)*CHAR_BIT, rhs);
	*this |= newRegister;  //TODO there should be a cleaner way...
	return *this;
}

/*BitRegister::~BitRegister(){
}*/

};	//End of namespace TBTK
