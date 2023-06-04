#pragma once
#include "ElevatorControllerBase.h"
#include "ElevatorMotor.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

using namespace std;

class ElevatorController : ElevatorControllerBase
{
public:
	void summonButtonPushed(int summoningFloor, ElevatorMotor::Direction direction);
	void floorButtonPushed(int destinationFloor);
	void reachedFloor(int floor);
	void startElevator();
	void requestInput();

private:
	struct callButtonDirections {
		bool up = false;
		bool down = false;
	};
	static const int FLOORS = 10;
	const string CALLSTRING = "call";
	const string EXITCODE = "exit";
	const string UP = "up";
	const string DOWN = "down";
	bool exit = false;
	int destinationFloor;
	vector<int> calls;
	map<int, callButtonDirections> floorCalls;
	int callsCount = 0;
	ElevatorMotor motor = ElevatorMotor();
	string elevatorInput;

	bool parse(string input);
	void nextFloor();
	void setDestination();
	void populateFloorCalls();
	bool floorCallsEmpty();
	void printElevatorText();
	void RunElevator();
};

