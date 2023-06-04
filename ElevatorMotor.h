#pragma once
class ElevatorMotor
{
public:
	ElevatorMotor();
	enum class Direction { Up = 1, None = 0, Down = -1 };
	int getCurrentFloor();
	Direction getCurrentDirection();
	void setCurrentDirection(Direction dir);
	void goToFloor(int floor);
private:
	Direction currentDirection;
	int currentFloor;
};