#pragma once
#include "ElevatorMotor.h"

class ElevatorControllerBase
{
public:
	// called when an up or down button was pushed on a floor
	virtual void summonButtonPushed(int summoningFloor, ElevatorMotor::Direction direction) = 0;
	// called when a button for a floor is pushed inside the car
	virtual void floorButtonPushed(int destinationFloor) = 0;
	// called when the car has reached a particular floor
	virtual void reachedFloor(int floor) = 0;
 	// virtual destructor
	virtual ~ElevatorControllerBase() {} 
};

