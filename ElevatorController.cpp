#include "ElevatorController.h"

void ElevatorController::summonButtonPushed(int summoningFloor, ElevatorMotor::Direction direction) {
	if (direction == ElevatorMotor::Direction::Up) {
		floorCalls.at(summoningFloor).up = true;
	}
	else {
		floorCalls.at(summoningFloor).down = true;
	}
}
void ElevatorController::floorButtonPushed(int destinationFloor) {
	calls.push_back(destinationFloor);
}


void ElevatorController::startElevator() {
	populateFloorCalls();
	while (true)
	{
		printElevatorText();
		if (exit)
		{
			break;
		}
		requestInput();
		RunElevator();
	}
}
void ElevatorController::RunElevator()
{
	reachedFloor(motor.getCurrentFloor());
	setDestination();
	nextFloor();
}
void ElevatorController::printElevatorText() {
	cout << "\nCurrent floor number: " << motor.getCurrentFloor() << '\n';
	cout << "Exit with: " << EXITCODE << '\n';
	cout << "Input floor 0-9 or call to floor with \"call\" followed by the floor number&direction or to continue just press enter\n";
}
void ElevatorController::requestInput() {

	getline(std::cin, elevatorInput);
	parse(elevatorInput);
}

//Handles user input
bool ElevatorController::parse(string input) {
	//clears spaces
	input.erase(remove_if(input.begin(), input.end(), ::isspace),input.end());
	//converts any uppercase to lowercase
	transform(input.begin(), input.end(),input.begin(), ::tolower);
	//if there is only one character and that character is a unique digit, adds to calls list
	if (input.length() == 1 
		&& isdigit(input[0]) 
		&& count(calls.begin(),calls.end(),input[0] - '0') < 1) {
		floorButtonPushed(input[0] - '0');
		return false;
	}
	
	//if there is a match for the string followed by a unique digit and a direction
	else if (input.substr(0, CALLSTRING.length()) == CALLSTRING) {
		if (CALLSTRING.length() + 1 <= input.length() && isdigit(input[CALLSTRING.length()])){
			int floor = input[CALLSTRING.length()] - '0';
			string direction = input.substr(CALLSTRING.length() + 1, CALLSTRING.length() + 1 + UP.length());
			if (input.length() == 7 && input.substr(CALLSTRING.length() + 1, CALLSTRING.length() + 1 + UP.length()) == UP) {
				summonButtonPushed(floor, ElevatorMotor::Direction::Up);
				return false;
			}
			else if (input.length() == 9 && input.substr(CALLSTRING.length() + 1, CALLSTRING.length() + 1 + DOWN.length()) == DOWN) {
				summonButtonPushed(floor, ElevatorMotor::Direction::Down);
				return false;
			}
		}
	}

	//checks for exit code
	else if (input.substr(0, EXITCODE.length()) == EXITCODE) {
		exit = true;
		return false;
	}
	//checks for continue
	else if (input.length() == 0) {
		return true;
	}
	cout << "Invalid input, try again: ";
	return false;
}

//Handles updating calls and floorCalls based on the currently reached floor
void ElevatorController::reachedFloor(int floor) {
	auto currentFloor = find(calls.begin(), calls.end(), floor);
	if (currentFloor != calls.end()) {
		calls.erase(currentFloor);
		calls.shrink_to_fit();
	}
	if (motor.getCurrentDirection() == ElevatorMotor::Direction::Down && floorCalls.at(floor).down) {
		floorCalls.at(floor).down = false;
	}
	if (motor.getCurrentDirection() == ElevatorMotor::Direction::Up && floorCalls.at(floor).up) {
		floorCalls.at(floor).up = false;
	}
	if (motor.getCurrentDirection() == ElevatorMotor::Direction::None) {
		floorCalls.at(floor).up = false;
		floorCalls.at(floor).down = false;
	}
	if (calls.empty() && floor == destinationFloor){
		floorCalls.at(floor).up = false;
		floorCalls.at(floor).down = false;
	}
	if(motor.getCurrentDirection() == ElevatorMotor::Direction::Up && !any_of(calls.begin(),calls.end(), [=](int c){return c > destinationFloor;})
	|| motor.getCurrentDirection() == ElevatorMotor::Direction::Down && !any_of(calls.begin(),calls.end(), [=](int c){return c < destinationFloor;})){
		floorCalls.at(floor).up = false;
		floorCalls.at(floor).down = false;
		if(floor == destinationFloor){
			motor.setCurrentDirection(ElevatorMotor::Direction::None);
		}
	}
}

//Handles setting the destination based on the current calls and floorCalls
void ElevatorController::setDestination() {
	ElevatorMotor::Direction currentDirection = motor.getCurrentDirection();

	//Handle no set direction
	if (currentDirection == ElevatorMotor::Direction::None) {
		//begins going to the call that was made first
		if (!calls.empty()) {
			destinationFloor = calls[0];
			if (motor.getCurrentFloor() < destinationFloor) motor.setCurrentDirection(ElevatorMotor::Direction::Up);
			else if (motor.getCurrentFloor() > destinationFloor) motor.setCurrentDirection(ElevatorMotor::Direction::Down);
		}
		//if there are no calls, goes to the lowest floorCall
		else if(!floorCallsEmpty()){
			for (pair<int, callButtonDirections> pair : floorCalls) {
				if (pair.second.up || pair.second.down) {
					destinationFloor = pair.first;
					if (motor.getCurrentFloor() > destinationFloor) {
						motor.setCurrentDirection(ElevatorMotor::Direction::Down);
					}
					else if (motor.getCurrentFloor() < destinationFloor) {
						motor.setCurrentDirection(ElevatorMotor::Direction::Up);
					}
					break;
				}
			}
		}
	}
	//if there are no set directives
	else if (calls.empty() && floorCallsEmpty()) motor.setCurrentDirection(ElevatorMotor::Direction::None);

	//Handle up direction
	else if (currentDirection == ElevatorMotor::Direction::Up) {
		if (!calls.empty() && !floorCallsEmpty()) {
			auto largestCall = max_element(calls.begin(), calls.end());
			int largestFloorCall = destinationFloor;
			for (int i = 0; i < FLOORS; i++) {
				if(floorCalls.at(i).up)
				largestFloorCall = i;
			}
			if (*largestCall > destinationFloor) {
				destinationFloor = *largestCall;
			}
			else if (largestFloorCall > destinationFloor) {
				destinationFloor = largestFloorCall;
			}
			else if (largestFloorCall < destinationFloor && *largestCall < destinationFloor) {
				if (!calls.empty()) {
					destinationFloor = *largestCall;
				}
				else {
					destinationFloor = largestFloorCall;
				}
				motor.setCurrentDirection(ElevatorMotor::Direction::Down);
			}
		}
		//If there are no calls and the largest floorCall is below, sets the direction to down
		else if (!calls.empty()) {
			auto largestCall = max_element(calls.begin(), calls.end());
			if (*largestCall > destinationFloor) {
				destinationFloor = *largestCall;
			}
			else if (*largestCall < destinationFloor) {
				destinationFloor = *largestCall;
				motor.setCurrentDirection(ElevatorMotor::Direction::Down);
			}
		}
		else if (!floorCallsEmpty()) {
			int largestFloorCall = destinationFloor;
			for (int i = 0; i < FLOORS; i++) {
				if (floorCalls.at(i).up)
					largestFloorCall = i;
			}
			if (largestFloorCall == destinationFloor) {
				for (int i = 0; i < FLOORS; i++) {
					if (floorCalls.at(i).down)
						largestFloorCall = i;
				}
			}
			if (largestFloorCall > destinationFloor) {
				destinationFloor = largestFloorCall;
			}
			else if (largestFloorCall < destinationFloor) {
				destinationFloor = largestFloorCall;
				motor.setCurrentDirection(ElevatorMotor::Direction::Down);
			}
		}
	}

	//Handle down direction
	else if (currentDirection == ElevatorMotor::Direction::Down) {
		if(!calls.empty() && !floorCallsEmpty()){
			auto smallestCall = min_element(calls.begin(), calls.end());
			int smallestFloorCall = destinationFloor;
			for (int i = FLOORS-1; i >= 0; i--) {
				if (floorCalls.at(i).down) 
				smallestFloorCall = i;
			}
			if (*smallestCall < destinationFloor) {
				destinationFloor = *smallestCall;
			}
			if (smallestFloorCall < destinationFloor) {
				destinationFloor = smallestFloorCall;
			}
			if (smallestFloorCall > destinationFloor && *smallestCall > destinationFloor) {
				if (!calls.empty()) {
					destinationFloor = *smallestCall;
				}
				else {
					destinationFloor = smallestFloorCall;
				}
				motor.setCurrentDirection(ElevatorMotor::Direction::Up);
			}
		}
		//If there are no calls and the smallest floorCall is below, sets the direction to up
		else if (!calls.empty()) {
			auto smallestCall = min_element(calls.begin(), calls.end());
			if (*smallestCall < destinationFloor) {
				destinationFloor = *smallestCall;
			}
			if (*smallestCall > destinationFloor) {
				destinationFloor = *smallestCall;
				motor.setCurrentDirection(ElevatorMotor::Direction::Up);
			}
		}
		else if (!floorCallsEmpty()) {
			int smallestFloorCall = destinationFloor;
			for (int i = FLOORS-1; i >= 0; i--) {
				if (floorCalls.at(i).down)
					smallestFloorCall = i;
			}
			if (smallestFloorCall == destinationFloor) {
				for (int i = FLOORS-1; i >= 0; i--) {
					if (floorCalls.at(i).up)
						smallestFloorCall = i;
				}
			}
			if (smallestFloorCall < destinationFloor) {
				destinationFloor = smallestFloorCall;
			}
			else if (smallestFloorCall > destinationFloor) {
				destinationFloor = smallestFloorCall;
				motor.setCurrentDirection(ElevatorMotor::Direction::Up);
			}
		}
	}
}

//Moves to the next floor in the direction of movement
void ElevatorController::nextFloor() {
	if (motor.getCurrentDirection() == ElevatorMotor::Direction::Up) {
		motor.goToFloor(motor.getCurrentFloor() + 1);
	}
	else if (motor.getCurrentDirection() == ElevatorMotor::Direction::Down) {
		motor.goToFloor(motor.getCurrentFloor() - 1);
	}
}

//Populates the floorCalls map
void ElevatorController::populateFloorCalls() {
	for (int i = 0; i < FLOORS; i++) {
		callButtonDirections dir;
		floorCalls.emplace(i, dir);
	}
}

//Checks if there are any floorCalls in any direction
bool ElevatorController::floorCallsEmpty() {
	for (int i = 0; i < FLOORS; i++) {
		if (floorCalls.at(i).up || floorCalls.at(i).down) {
			return false;
		}
	}
	return true;
}