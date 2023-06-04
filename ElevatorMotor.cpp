#include "ElevatorMotor.h"

ElevatorMotor::ElevatorMotor() :
currentDirection(Direction::None),
currentFloor(0)
{

}
int ElevatorMotor::getCurrentFloor() {
	return currentFloor;
}
ElevatorMotor::Direction ElevatorMotor::getCurrentDirection() {
	return currentDirection;
}
void ElevatorMotor::goToFloor(int floor) { 
	currentFloor = floor;
}
void ElevatorMotor::setCurrentDirection(Direction dir) {
	currentDirection = dir;
}