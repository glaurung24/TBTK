#include "TBTK/BitRegister.h"
#include "TBTK/LadderOperator.h"
#include "TBTK/Model.h"

#include "gtest/gtest.h"

namespace TBTK{

class LadderOperatorTest : public ::testing::Test{
protected:
	Model model;
	LadderOperator fermionicCreation[2];
	LadderOperator fermionicAnnihilation[2];
	LadderOperator bosonicCreation[2];
	LadderOperator bosonicAnnihilation[2];
	const unsigned int NUM_STATES = 5;

	void SetUp() override{
		for(unsigned int n = 0; n < NUM_STATES; n++)
			model << HoppingAmplitude(1, {n}, {n});
		model.construct();

		FockState templateState(NUM_STATES);
		BitRegister fermionMask;
		for(unsigned int n = 0; n < fermionMask.size(); n++)
			fermionMask.set(n, true);

		for(unsigned int n = 0; n < 2; n++){
			fermionicCreation[n] = LadderOperator(
				LadderOperator::Type::Creation,
				Statistics::FermiDirac,
				&model.getHoppingAmplitudeSet(),
				2+n,	//State with Hilbert space index 3.
				1,	//Fermionic = 1.
				1,	//Max occupation.
				templateState,
				fermionMask
			);
			fermionicAnnihilation[n] = LadderOperator(
				LadderOperator::Type::Annihilation,
				Statistics::FermiDirac,
				&model.getHoppingAmplitudeSet(),
				2+n,	//State with Hilbert space index 3.
				1,	//Fermionic = 1.
				1,	//Max occupation.
				templateState,
				fermionMask
			);
			bosonicCreation[n] = LadderOperator(
				LadderOperator::Type::Creation,
				Statistics::BoseEinstein,
				&model.getHoppingAmplitudeSet(),
				2+n,	//State with Hilbert space index 3.
				2,	//Up to three particles.
				3,	//MaxOccupation.
				templateState,
				fermionMask
			);
			bosonicAnnihilation[n] = LadderOperator(
				LadderOperator::Type::Annihilation,
				Statistics::BoseEinstein,
				&model.getHoppingAmplitudeSet(),
				2+n,	//State with Hilbert space index 3.
				2,	//Up to three particles.
				3,	//Max occupation.
				templateState,
				fermionMask
			);
		}
	}
};

//TBTKFeature ManyParticle.LadderOperator.getType.1 2019-11-03
TEST_F(LadderOperatorTest, getType1){
	EXPECT_EQ(
		fermionicCreation[0].getType(),
		LadderOperator::Type::Creation
	);
}

//TBTKFeature ManyParticle.LadderOperator.getType.2 2019-11-03
TEST_F(LadderOperatorTest, getType2){
	EXPECT_EQ(
		fermionicAnnihilation[0].getType(),
		LadderOperator::Type::Annihilation
	);
}

//TBTKFeature ManyParticle.LadderOperator.getType.3 2019-11-03
TEST_F(LadderOperatorTest, getType3){
	EXPECT_EQ(
		bosonicCreation[0].getType(),
		LadderOperator::Type::Creation
	);
}

//TBTKFeature ManyParticle.LadderOperator.getType.4 2019-11-03
TEST_F(LadderOperatorTest, getType4){
	EXPECT_EQ(
		bosonicAnnihilation[0].getType(),
		LadderOperator::Type::Annihilation
	);
}

//TBTKFeature ManyParticle.LadderOperator.getPhysicalIndex.1 2019-11-03
TEST_F(LadderOperatorTest, getPhysicalIndex1){
	EXPECT_TRUE(fermionicCreation[0].getPhysicalIndex().equals({2}));
}

//TBTKFeature ManyParticle.LadderOperator.getState.1 2019-11-03
TEST_F(LadderOperatorTest, getState1){
	EXPECT_EQ(fermionicCreation[0].getState(), 2);
}

//TBTKFeature ManyParticle.LadderOperator.getNumParticles.1 2019-11-03
TEST_F(LadderOperatorTest, getNumParticles1){
	FockState fockState(NUM_STATES);
	EXPECT_EQ(fermionicCreation[0].getNumParticles(fockState), 0);

	fockState.getBitRegister() = (unsigned) 0x00000004;
	EXPECT_EQ(fermionicCreation[0].getNumParticles(fockState), 1);
}

//TBTKFeature ManyParticle.LadderOperator.getNumParticles.2 2019-11-03
TEST_F(LadderOperatorTest, getNumParticles2){
	FockState fockState(NUM_STATES);
	EXPECT_EQ(bosonicCreation[0].getNumParticles(fockState), 0);

	fockState.getBitRegister() = (unsigned) 0x00000010;
	EXPECT_EQ(bosonicCreation[0].getNumParticles(fockState), 1);

	fockState.getBitRegister() = (unsigned) 0x00000020;
	EXPECT_EQ(bosonicCreation[0].getNumParticles(fockState), 2);

	fockState.getBitRegister() = (unsigned) 0x00000030;
	EXPECT_EQ(bosonicCreation[0].getNumParticles(fockState), 3);
}

//TBTKFeature ManyParticle.LadderOperator.operatorMultiplication.1 2019-11-03
TEST_F(LadderOperatorTest, operatorMultiplication1){
	FockState fockState(NUM_STATES);

	fermionicCreation[0]*fockState;
	EXPECT_EQ(fermionicCreation[0].getNumParticles(fockState), 1);
	EXPECT_EQ(fockState.getPrefactor(), 1);

	fermionicCreation[0]*fockState;
	EXPECT_TRUE(fockState.isNull());
}

//TBTKFeature ManyParticle.LadderOperator.operatorMultiplication.2 2019-11-03
TEST_F(LadderOperatorTest, operatorMultiplication2){
	FockState fockState(NUM_STATES);

	fermionicCreation[0]*fockState;
	fermionicAnnihilation[0]*fockState;
	EXPECT_EQ(fermionicCreation[0].getNumParticles(fockState), 0);
	EXPECT_EQ(fockState.getPrefactor(), 1);

	fermionicAnnihilation[0]*fockState;
	EXPECT_TRUE(fockState.isNull());
}

//TBTKFeature ManyParticle.LadderOperator.operatorMultiplication.3 2019-11-03
TEST_F(LadderOperatorTest, operatorMultiplication3){
	FockState fockState(NUM_STATES);

	fermionicCreation[0]*fockState;
	fermionicCreation[1]*fockState;
	EXPECT_EQ(fockState.getPrefactor(), 1);
}

//TBTKFeature ManyParticle.LadderOperator.operatorMultiplication.4 2019-11-03
TEST_F(LadderOperatorTest, operatorMultiplication4){
	FockState fockState(NUM_STATES);

	fermionicCreation[1]*fockState;
	fermionicCreation[0]*fockState;
	EXPECT_EQ(fockState.getPrefactor(), -1);
}

//TBTKFeature ManyParticle.LadderOperator.operatorMultiplication.5 2019-11-03
TEST_F(LadderOperatorTest, operatorMultiplication5){
	FockState fockState(NUM_STATES);

	fermionicCreation[0]*fockState;
	fermionicCreation[1]*fockState;
	fermionicAnnihilation[1]*fockState;
	fermionicAnnihilation[0]*fockState;
	EXPECT_EQ(fockState.getPrefactor(), 1);
}

//TBTKFeature ManyParticle.LadderOperator.operatorMultiplication.6 2019-11-03
TEST_F(LadderOperatorTest, operatorMultiplication6){
	FockState fockState(NUM_STATES);

	fermionicCreation[0]*fockState;
	fermionicCreation[1]*fockState;
	fermionicAnnihilation[0]*fockState;
	fermionicAnnihilation[1]*fockState;
	EXPECT_EQ(fockState.getPrefactor(), -1);
}

//TBTKFeature ManyParticle.LadderOperator.operatorMultiplication.7 2019-11-03
TEST_F(LadderOperatorTest, operatorMultiplication7){
	FockState fockState(NUM_STATES);

	int factorial = 1;
	for(unsigned int n = 0; n < 4; n++){
		EXPECT_EQ(bosonicCreation[0].getNumParticles(fockState), n);
		bosonicCreation[0]*fockState;
		factorial *= (n+1);
	}
	EXPECT_TRUE(fockState.isNull());
}

//TBTKFeature ManyParticle.LadderOperator.operatorMultiplication.7 2019-11-03
TEST_F(LadderOperatorTest, operatorMultiplication8){
	FockState fockState(NUM_STATES);

	for(unsigned int n = 0; n < 3; n++)
		bosonicCreation[0]*fockState;

	for(unsigned int n = 0; n < 4; n++){
		EXPECT_EQ(bosonicCreation[0].getNumParticles(fockState), 3-n);
		bosonicAnnihilation[0]*fockState;
	}

	EXPECT_TRUE(fockState.isNull());
}

};
