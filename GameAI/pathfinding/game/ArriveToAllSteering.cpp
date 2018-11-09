#include <cassert>

#include "ArriveToAllSteering.h"
#include "FaceSteering.h"
#include "ArriveSteering.h"
#include "Game.h"
#include "UnitManager.h"
#include "Unit.h"

ArriveToAllSteering::ArriveToAllSteering(const UnitID & ownerID, std::vector<Vector2D> targetVector, const UnitID& targetID = INVALID_UNIT_ID, const float targetRadius = 0, const float slowRadius = 100, const float timeToTarget = 0.1)
{
	mTargetVector = targetVector;
	mTargetRadius = targetRadius;
	mTimeToTarget = timeToTarget;
	mSlowRadius = slowRadius;
	index = 0;
	setOwnerID(ownerID);
	setTargetID(targetID);
	if(targetVector.size() == 0){
		setTargetLoc(ZERO_VECTOR2D);
		mpFaceSteering = new FaceSteering(ownerID, ZERO_VECTOR2D, targetID);
		mpArriveSteering = new ArriveSteering(ownerID, ZERO_VECTOR2D, targetID, targetRadius, slowRadius, timeToTarget);
	}
	else {
		setTargetLoc(targetVector[0]);
		mpFaceSteering = new FaceSteering(ownerID, targetVector[0], targetID);
		mpArriveSteering = new ArriveSteering(ownerID, targetVector[0], targetID, targetRadius, slowRadius, timeToTarget);
	}
}

ArriveToAllSteering::~ArriveToAllSteering()
{
	delete mpFaceSteering;
	delete mpArriveSteering;
}

Steering * ArriveToAllSteering::getSteering()
{
	Unit* pOwner = gpGame->getUnitManager()->getUnit(mOwnerID);
	PhysicsData data = pOwner->getPhysicsComponent()->getData();

	data = mpArriveSteering->getSteering()->getData();
	if(mpArriveSteering->finishedSteering){
		if(index + 1 >= mTargetVector.size()){
			return this;
		}
		ArriveAtNewPoint();
		mpArriveSteering->finishedSteering = false;
	}
	auto faceData = mpFaceSteering->getSteering()->getData();
	data.rotAcc = faceData.rotAcc;
	data.rotVel = faceData.rotVel;

	this->mData = data;
	return this;
}

void ArriveToAllSteering::ArriveAtNewPoint(){
	index++;
	delete mpFaceSteering;
	delete mpArriveSteering;
	mpFaceSteering = new FaceSteering(mOwnerID, mTargetVector[index], mTargetID);
	mpArriveSteering = new ArriveSteering(mOwnerID, mTargetVector[index], mTargetID, mTargetRadius, mSlowRadius, mTimeToTarget);
}