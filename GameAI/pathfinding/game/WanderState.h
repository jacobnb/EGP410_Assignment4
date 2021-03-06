#pragma once

#include "StateMachine.h"

class WanderState : public StateMachineState
{
public:
	WanderState( const SM_idType& id , bool player, int ownerID, int followRange = 250):StateMachineState(id), mPlayer(player), mOwnerID(ownerID), mFollowRange(followRange){};

	virtual void onEntrance();
	virtual void onExit();
	virtual StateTransition* update();

private:
	bool mPlayer;
	bool currentlyFighting = false;
	int mOwnerID;
	int mFollowRange;
};