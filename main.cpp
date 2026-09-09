#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>


// сделать going_up полем elevator


struct Passenger {
	int target_floor;
	bool is_going_up;
};


class Floor {
private:
	std::vector<Passenger> passengers;
	int floor_num;
public:
	Floor(std::vector<Passenger>& input_passengers, int floor_number) {
		passengers = input_passengers;
		floor_num = floor_number;
	};

	std::vector<Passenger> get_passengers() const {
		return passengers;
	}

	void add_passenger(Passenger& passenger) {
		passengers.push_back(passenger);
	}

	int get_floor_number() const {
		return floor_num;
	}

	std::vector<Passenger> remove_passenger(Passenger& passenger) {
		for (int i = 0; i < passengers.size(); i++) {
			if (passengers[i].target_floor == passenger.target_floor) {
				passengers.erase(passengers.begin() + i);
				break;
			}
		}

		return passengers;
	}

	bool passengers_check() { // нужно ли кому-то уехать с этажа
		for (auto& i : passengers) {
			if (i.target_floor != floor_num) {
				return true;
			}
		}

		return false;
	}

	int find_passenger(Passenger& passenger) {
		int i;
		for (i = 0; i < passengers.size(); i++) {
			if (passenger.target_floor == passengers[i].target_floor) {
				break;
			}
		}

		return i;
	}
};


class Building {
private:
	std::vector<Floor> floors;

public:
	Building(std::vector<Floor>& input_floors) {
		floors = input_floors;
	}

	Building() = default;

	std::vector<Floor> getFloors() const {
		return floors;
	}

	bool movePeople(std::vector<Passenger>& passengers, int& curr_floor, int& target_floor) {
		// скорее всего не понадобится
		for (int i = 0; i < passengers.size(); i++) {
			int person_index = floors[curr_floor].find_passenger(passengers[i]);
			if (person_index != floors.size()) {
				floors.erase(floors.begin() + curr_floor);
				floors[target_floor].add_passenger(passengers[i]);
			}
			else {
				return false;
			}
		}

		return true;
	}

	bool check_passengers_waiting() { // нужно ли еще кого-то отвезти
		for (auto& floor : floors) {
			if (floor.passengers_check()) {
				return true;
			}
		}

		return false;
	}

	void take_person(int floor, Passenger person) { // взять человека на этаж
		floors[floor].add_passenger(person);
	}

	void list_people() {
		for (int i = floors.size() - 1; i >= 0; i--) {
			std::cout << "floor " << i << ": ";
			for (auto& passenger : floors[i].get_passengers()) {
				std::cout << passenger.target_floor << ' ';
			}
			std::cout << '\n';
		}
	}

	void remove_people(std::vector<Passenger> people, int floor) {
		for (auto& person : people) {
			floors[floor].remove_passenger(person);
		}
	}
};


class Elevator {
private:
	int curr_floor;
	int people_count;
	std::vector<Passenger> passengers;
	Building building;
public:
	Elevator(std::vector<Floor> floors) {
		building = Building(floors);
		curr_floor = 0;
		people_count = 0;
	};

	Elevator() = default;

	Elevator(int floor_input, int count_input, Building building_input) {
		curr_floor = floor_input;
		people_count = count_input;
		building = building_input;
	}

	int get_curr_floor() {
		return curr_floor;
	}

	int get_people_count() { return people_count; }

	Building get_building() {
		return building;
	}

	std::vector<Passenger> get_passengers() {
		return passengers;
	}

	int go_to_floor(int floor) { // приехать на этаж
		int moves = std::abs(curr_floor - floor);
		curr_floor = floor;
		return moves;
	}

	void take_passengers(bool going_up) {
		// берем вектор пассажиров на этом этаже
		std::vector<Passenger> people_on_floor = building.getFloors()[curr_floor].get_passengers(); 

		for (auto& passenger : people_on_floor) {

			if (( passenger.target_floor != curr_floor) &&
				(passenger.is_going_up == going_up && people_count < 3)) {
				passengers.push_back(passenger);
				people_count++;

				building.remove_people({ passenger }, curr_floor);
			}
		}
	}

	void leave_passenger(Passenger passenger) { // оставить пассажира на этаже
		for (int i = 0; i < passengers.size(); i++) {
			if (passengers[i].target_floor == passenger.target_floor) {
				passengers.erase(passengers.begin() + i);
				people_count--;
				building.take_person(curr_floor, passenger);
				return;
			}
		}
	}

	bool better_up_or_down() { // 1 - вверх 0 - вниз
		int up_count = 0;
		int down_count = 0;
		for (auto& floor : building.getFloors()) {
			if (floor.passengers_check()) {
				if (floor.get_floor_number() < curr_floor) {
					down_count++;
				}
				else if (floor.get_floor_number() > curr_floor) {
					up_count++;
				}
			}
		}

		return up_count > down_count;
	}

	void test_remove() {
		auto test_person = building.getFloors()[0].get_passengers()[0];
		int size_before = building.getFloors()[0].get_passengers().size();
		building.remove_people({ test_person }, 0);
		if (building.getFloors()[0].get_passengers().size() == size_before) {
			std::cout << "test failed" << '\n';
		}
	}
};


class Solution {
private:
	Elevator elevator;
	int count_moves = 0;
	int people_count = 0;
public:
	Solution(Elevator& elevator_input) {
		elevator = Elevator(elevator_input);
	}

	Solution(std::vector<std::vector<Passenger>> input) {
		load_test_data(input);
	}

	Solution() = default;

	Elevator get_elevator() {
		return elevator;
	}

	void count_people() {
		std::vector<Floor> floors = elevator.get_building().getFloors();
		for (auto& floor : floors) {
			people_count += floor.get_passengers().size();
		}
	}

	void generate_test() {
		std::srand(time(nullptr));
		people_count = 0;
		int floor_count = 2 + std::rand() % 9;
		std::vector<Floor> floors;

		for (int i = 0; i < floor_count; i++) {
			int people_count = 0 + std::rand() % 10;
			std::vector<Passenger> people;

			for (int j = 0; j < people_count; j++) {
				Passenger passenger;
				passenger.target_floor = std::rand() % floor_count;
				passenger.is_going_up = passenger.target_floor > i;
				people.push_back(passenger);
			}

			Floor floor = Floor(people, i);
			floors.push_back(floor);
		}

		elevator = Elevator(floors);
	}

	void take_people_on_the_way(int target_floor, bool going_up) { // закончить эту перегрузку, дописать это решение
		auto passengers_on_elevator = elevator.get_passengers();
		auto floors = elevator.get_building().getFloors();

		// развозим, забираем людей наверху
		if (going_up) {
			for (int i = elevator.get_curr_floor(); i < target_floor; i++) {
				std::vector<Passenger> people_on_floor = floors[i].get_passengers();

				for (auto& person : people_on_floor) {
					if (person.target_floor <= target_floor &&
						person.target_floor != i &&
						people_count < 3) {
						count_moves += elevator.go_to_floor(i);
						process_floor(i);
						elevator.take_passengers(going_up);
					}
				}
			}
		}
		else {
			for (int i = elevator.get_curr_floor(); i > target_floor; i--) {
				std::vector<Passenger> people_on_floor = floors[i].get_passengers();

				for (auto& person : people_on_floor) {
					if (person.target_floor >= target_floor &&
						person.target_floor != i &&
						people_count < 3) {
						count_moves += elevator.go_to_floor(i);
						process_floor(i);
						elevator.take_passengers(going_up);
					}
				}
			}
		}
	}

	void process_floor(int curr_floor) {
		auto passengers = elevator.get_passengers();
		for (auto& passenger : passengers) {
			if (passenger.target_floor == curr_floor) {
				elevator.leave_passenger(passenger);
			}
		}
	}

	void log(bool log_moves) {
		if (log_moves) {
			std::cout << '\n' << "at floor " << elevator.get_curr_floor() << '\n';
			std::cout << "moves count: " << count_moves << '\n';
			std::cout << "passengers (" << elevator.get_passengers().size() << "):" << '\n';
			for (auto& passenger : elevator.get_passengers()) {
				std::cout << "passenger: to floor " << passenger.target_floor << '\n';
			}
		}
	}

	int find_the_furtherst_floor(bool going_up) { // найти самый дальний этаж, где люди хотят ехать в нашем направлении
		// если не нашел такой этаж, возвращает текущий этаж
		int curr_floor = elevator.get_curr_floor();
		auto floors = elevator.get_building().getFloors();
		int furtherst_floor = curr_floor;

		if (going_up) {
			for (int i = curr_floor + 1; i < floors.size(); i++) {
				auto people_on_floor = floors[i].get_passengers();
				for (auto& person : people_on_floor) {
					if (person.is_going_up != going_up) { // человек едет противоположно текущему направлению лифта
						furtherst_floor = i;
					}
				}
			}
		}
		else {
			for (int i = curr_floor - 1; i >= 0; i--) {
				auto people_on_floor = floors[i].get_passengers();
				for (auto& person : people_on_floor) {
					if (person.is_going_up != going_up) {
						furtherst_floor = i;
					}
				}
			}
		}

		return furtherst_floor;
	}

	// по поводу алгоритма:
	// лифт будет ездить вверх и вниз до тех пор, пока будут люди, которых нужно куда-то отвезти.
	// по умолчанию наша конечная точка - это человек на самом верхнем этаже, которому куда-то надо
	// и пока мы едем за этим человеком, можем брать людей по пути, которым надо наверх, на этаж
	// ниже, где стоит этот человек, и развозить
	void solve(bool log_moves) {
		Building building = elevator.get_building();
		std::vector<Floor> floors = building.getFloors();
		count_moves = 0;
		count_people();
		bool going_up = true;
		int target_floor = 0;

		while(building.check_passengers_waiting()){
			target_floor = find_the_furtherst_floor(going_up);

			if (target_floor != elevator.get_curr_floor()) {
				if (floors[elevator.get_curr_floor()].passengers_check()) {
					elevator.take_passengers(going_up);
				}

				std::cout << "**************************" << '\n';
				std::cout << "target floor: " << target_floor << '\n';
				std::cout << "**************************" << '\n';

				floors = building.getFloors();
				take_people_on_the_way(target_floor, going_up);
				log_state();

				elevator.go_to_floor(target_floor);
				process_floor(target_floor);
				floors = building.getFloors();
				elevator.take_passengers(!going_up);
				log_state();
			}

			going_up = !going_up;
		}
	}

	void load_test_data(std::vector<std::vector<Passenger>>& input) { // загрузить тестовые данные вручную
		std::vector<Floor> floors;
		for (int i = 0; i < input.size(); i++) {
			floors.push_back(Floor(input[i], i));
		}

		elevator = Elevator(floors);
	}

	void log_state() {
		std::vector<Floor> floors = elevator.get_building().getFloors();
		std::cout << "----------------------------" << '\n';

		for (int i = floors.size() - 1; i >= 0; i--) {
			std::cout << "Floor " << i << '\n';
			for (auto& person : floors[i].get_passengers()) {
				std::cout << '\t' << "target floor " << person.target_floor << '\n';
			}
		}

		std::cout << "Elevator:" << '\n';
		std::cout << "curr_floor: " << elevator.get_curr_floor() << '\n';
		std::cout << "passengers:" << '\n';
		auto passengers = elevator.get_passengers();
		for (auto& passenger : passengers) {
			std::cout << "    target floor: " << passenger.target_floor << '\n';
		}

		std::cout << '\n' << "total moves: " << count_moves << '\n';
		std::cout << "people count: " << people_count << '\n';
	}

};


int main() { // дописать до возможного пути
	//std::vector<Floor> floors;
	//std::vector<Passenger> input_floor = { {1, true}, {2, true} };
	//Floor floor = Floor(input_floor, 0);
	//floors.push_back(floor);

	//input_floor = { {0, false}, {2, true} };
	//floor = Floor(input_floor, 1);
	//floors.push_back(floor);

	//input_floor = { {0, false}, {1, false} };
	//floor = Floor(input_floor, 2);
	//floors.push_back(floor);

	//Elevator elevator = Elevator(floors);
	//elevator.test_remove();

	//Solution solution = Solution();
	//solution.generate_test();

	//solution.log_state();
	//solution.solve();
	//solution.log_state();


	// ошибочный тест, куда-то пропадают 2 человека
	std::vector<std::vector<Passenger>> solution_input = { {}, {{0, false}},
		{{2, false}, {2, false}, {2, false}, {0, false}, {0, false}, {2, false}} };

	Solution solution = Solution(solution_input);
	solution.log_state();
	solution.solve(true);
	solution.log_state();

	return 0;
}