#include "TBTK/BitRegister.h"
#include "TBTK/FockStateMap/FockStateMap.h"

#include "gtest/gtest.h"

namespace TBTK{
namespace FockStateMap{

class ImplementedFockStateMap : public FockStateMap{
public:
	ImplementedFockStateMap(
		unsigned int exponentialDimension
	) : FockStateMap(exponentialDimension){}

	//Dummy implementation to make the class non-abstract.
	virtual unsigned int getBasisSize() const{
		return 0;
	}

	//Dummy implementation to make the class non-abstract.
	virtual unsigned int getBasisIndex(
		const FockState &fockState
	) const{
		return 0;
	}

	//Dummy implementation to make the class non-abstract.
	virtual FockState getFockState(unsigned int index) const{
		return FockState(1);
	}
};

//TBTKFeature FockStateMap.FockStateMap.getExponentialDimension.1 2019-11-04
TEST(FockStateMap, getExponentialDimension1){
	ImplementedFockStateMap implementedFockStateMap(100);
	EXPECT_EQ(implementedFockStateMap.getExponentialDimension(), 100);
}

};
};
