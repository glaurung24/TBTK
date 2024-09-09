#include "TBTK/FockStateRule/DifferenceRule.h"
#include "TBTK/FockStateRule/SumRule.h"
#include "TBTK/FockStateRule/FockStateRule.h"
#include "TBTK/FockStateRuleSet.h"
#include "TBTK/Model.h"

#include "gtest/gtest.h"

namespace TBTK{
namespace FockStateRule{

class FockStateRuleSetTest : public ::testing::Test{
protected:
	Model model;
	FockSpace fockSpace0;

	void SetUp() override{
		for(unsigned int n = 0; n < 4; n++)
			model << HoppingAmplitude(1, {n}, {n});
		model.construct();

		fockSpace0 = FockSpace(
			&model.getHoppingAmplitudeSet(),
			Statistics::FermiDirac,
			1
		);
	}
};

//TBTKFeature ManyParticle.FockStateRuleSet.clone.1 2019-11-05
TEST_F(FockStateRuleSetTest, isSatisfied1){
	FockStateRuleSet fockStateRuleSet;
	fockStateRuleSet.addFockStateRule(std::make_shared<FockStateRule>(
		SumRule({{0}, {1}, {2}, {3}}, 3)));
	fockStateRuleSet.addFockStateRule(std::make_shared<FockStateRule>(
		DifferenceRule({{0}, {1}}, {{2}, {3}}, 1)
	));

	FockState fockState(model.getBasisSize());
	fockState.getBitRegister().set(0, 1);
	fockState.getBitRegister().set(1, 1);
	fockState.getBitRegister().set(2, 1);

	EXPECT_TRUE(fockStateRuleSet.isSatisfied(fockSpace0, fockState));
}

//TBTKFeature ManyParticle.FockStateRuleSet.clone.2 2019-11-05
TEST_F(FockStateRuleSetTest, isSatisfied2){
	FockStateRuleSet fockStateRuleSet;
	fockStateRuleSet.addFockStateRule(std::make_shared<FockStateRule>(
		SumRule({{0}, {1}, {2}, {3}}, 3)));
	fockStateRuleSet.addFockStateRule(std::make_shared<FockStateRule>(
		DifferenceRule({{0}, {1}}, {{2}, {3}}, 1))
	);

	FockState fockState(model.getBasisSize());
	fockState.getBitRegister().set(0, 1);
	fockState.getBitRegister().set(2, 1);
	fockState.getBitRegister().set(3, 1);

	EXPECT_FALSE(fockStateRuleSet.isSatisfied(fockSpace0, fockState));
}

//TBTKFeature ManyParticle.FockStateRuleSet.clone.3 2019-11-05
TEST_F(FockStateRuleSetTest, isSatisfied3){
	FockStateRuleSet fockStateRuleSet;
	fockStateRuleSet.addFockStateRule(std::make_shared<FockStateRule>(
		SumRule({{0}, {1}, {2}, {3}}, 3)));
	fockStateRuleSet.addFockStateRule(std::make_shared<FockStateRule>(
		DifferenceRule({{0}, {1}}, {{2}, {3}}, 1))
	);

	FockState fockState(model.getBasisSize());
	fockState.getBitRegister().set(0, 1);

	EXPECT_FALSE(fockStateRuleSet.isSatisfied(fockSpace0, fockState));
}

//TBTKFeature ManyParticle.FockStateRuleSet.addFockStateRule.1 2019-11-05
//TBTKFeature ManyParticle.FockStateRuleSet.getSize.1 2019-11-05
TEST(FockStateRuleSet, addFockStateRule){
	FockStateRuleSet fockStateRuleSet;
	fockStateRuleSet.addFockStateRule(std::make_shared<FockStateRule>(
		SumRule({{0}, {1}}, 1)));
	fockStateRuleSet.addFockStateRule(std::make_shared<FockStateRule>(
		SumRule({{1}, {2}}, 1)));
	EXPECT_EQ(fockStateRuleSet.getSize(), 2);
}

//TBTKFeature ManyParticle.FockStateRuleSet.operatorEqual.1 2019-11-05
TEST_F(FockStateRuleSetTest, operatorEqual1){
	FockStateRuleSet fockStateRuleSet0;
	fockStateRuleSet0.addFockStateRule(std::make_shared<FockStateRule>(
		SumRule({{0}, {1}, {2}, {3}}, 3)));
	fockStateRuleSet0.addFockStateRule(std::make_shared<FockStateRule>(
		DifferenceRule({{0}, {1}}, {{2}, {3}}, 1))
	);

	FockStateRuleSet fockStateRuleSet1;
	fockStateRuleSet1.addFockStateRule(std::make_shared<FockStateRule>(
		SumRule({{0}, {1}, {2}, {3}}, 3)));
	fockStateRuleSet1.addFockStateRule(std::make_shared<FockStateRule>(
		DifferenceRule({{0}, {1}}, {{2}, {3}}, 1))
	);

	EXPECT_EQ(fockStateRuleSet0, fockStateRuleSet1);
}

//TBTKFeature ManyParticle.FockStateRuleSet.operatorEqual.1 2019-11-05
TEST_F(FockStateRuleSetTest, operatorEqual2){
	FockStateRuleSet fockStateRuleSet0;
	fockStateRuleSet0.addFockStateRule(std::make_shared<FockStateRule>(
		SumRule({{0}, {1}, {2}, {3}}, 3)));
	fockStateRuleSet0.addFockStateRule(std::make_shared<FockStateRule>(
		DifferenceRule({{0}, {1}}, {{2}, {3}}, 1)
	));

	FockStateRuleSet fockStateRuleSet1;
	fockStateRuleSet1.addFockStateRule(std::make_shared<FockStateRule>(
		SumRule({{0}, {1}, {2}, {3}}, 3)));
	fockStateRuleSet1.addFockStateRule(std::make_shared<FockStateRule>(
		DifferenceRule({{0}, {1}}, {{2}, {3}}, 2)
	));

	EXPECT_FALSE(fockStateRuleSet0 == fockStateRuleSet1);
}

// //TBTKFeature ManyParticle.FockStateRuleSet.operatorMultiplication.1 2019-11-05
// TEST_F(FockStateRuleSetTest, operatorMultiplication1){
// 	FockState templateState(model.getBasisSize());
// 	BitRegister fermionMask(model.getBasisSize()+1);
// 	LadderOperator ladderOperator(
// 		LadderOperator::Type::Annihilation,
// 		Statistics::FermiDirac,
// 		&model.getHoppingAmplitudeSet(),
// 		0,
// 		1,
// 		1,
// 		templateState,
// 		fermionMask
// 	);

// 	FockStateRuleSet fockStateRuleSet0;
// 	fockStateRuleSet0.addFockStateRule(std::make_shared<FockStateRule>(
// 		SumRule({{0}, {1}, {2}, {3}}, 3)));
// 	fockStateRuleSet0.addFockStateRule(
// 		std::make_shared<FockStateRule>(DifferenceRule({{0}, {1}}, {{2}, {3}}, 1))
// 	);

// 	FockStateRuleSet fockStateRuleSet1;
// 	fockStateRuleSet1.addFockStateRule(
// 		ladderOperator*SumRule({{0}, {1}, {2}, {3}}, 3)
// 	);
// 	fockStateRuleSet1.addFockStateRule(
// 		ladderOperator*DifferenceRule({{0}, {1}}, {{2}, {3}}, 1)
// 	);

// 	EXPECT_EQ(ladderOperator*fockStateRuleSet0, fockStateRuleSet1);
// }


};
};
