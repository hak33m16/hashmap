////////////////////////////////////////////////////////
//
// implementation basic hashmap (unordered container)
// Mikhail Nesterenko: adapted from Proc C++
// 4/15/2014
//
////////////////////////////////
//
// Abdel-Hakeem Badran
// 02/27/2018
//
// Implemented tests for:
// insert(), erase(), and rehash()
//
///////////////////

#include "hashmap.h"
#include <iostream>
#include <string>

using std::cin; using std::cout; using std::endl;
using std::string;

int main() {

	hashmap<int, int> myHash;
	std::cout << "Declared empty hash: myHash\n";

	std::cout << "## BEGINNING INSERTION TESTS ##\n";

	auto insert_return = myHash.insert(pair<int, int>(4, 40));
	std::cout << "Attempted to insert pair (4, 40) int myHash\n";
	std::cout << "Pair recieved back: (" << insert_return.first << ", " << insert_return.second << ")\n";
	string msg = insert_return.second ? "Looks like it hasn't been inserted before!\n\n" : "Tried to insert a value that already exists.\n\n";
	std::cout << msg;

	insert_return = myHash.insert(make_pair(6, 60));
	std::cout << "Attempted to insert pair (6, 60) int myHash\n";
	std::cout << "Pair recieved back: (" << insert_return.first << ", " << insert_return.second << ")\n";
	msg = insert_return.second ? "Looks like it hasn't been inserted before!\n\n" : "Tried to insert a value that already exists.\n\n";
	std::cout << msg;

	insert_return = myHash.insert(make_pair(6, 60));
	std::cout << "Attempted to insert pair (6, 60) int myHash\n";
	std::cout << "Pair recieved back: (" << insert_return.first << ", " << insert_return.second << ")\n";
	msg = insert_return.second ? "Looks like it hasn't been inserted before!\n\n" : "Tried to insert a value that already exists.\n\n";
	std::cout << msg;

	std::cout << "## BEGINNING ERASE TESTS ##\n";
	std::cout << "Note that memory locations can change due to list/vector resizing!\n";

	auto erase_return = myHash.erase(4);
	std::cout << "Attempting to erase the element with key 4\n";
	std::cout << "Pair recieved back: (" << erase_return.first << ", " << erase_return.second << ")\n";
	if (erase_return.first == nullptr) {
		std::cout << "No element exists after 4. We were returned a nullptr.\n";
	}
	else {
		std::cout << "There is an element after 4: (Key: " << (erase_return.first)->first << ", Value: " << (erase_return.first)->second << ")\n";
	}
	std::cout << "\n";

	erase_return = myHash.erase(6);
	std::cout << "Attempting to erase the element with key 6\n";
	std::cout << "Pair recieved back: (" << erase_return.first << ", " << erase_return.second << ")\n";
	if (erase_return.first == nullptr) {
		std::cout << "No element exists after 6. We were returned a nullptr.\n";
	}
	else {
		std::cout << "There is an element after 6: (Key: " << (erase_return.first)->first << ", Value: " << (erase_return.first)->second << ")\n";
	}
	std::cout << "\n";

	std::cout << "Inserting a bunch of pairs: (4, 40), (6, 60), and (5, 40)\n";

	myHash.insert(pair<int, int>(4, 40));
	myHash.insert(pair<int, int>(6, 60));
	myHash.insert(pair<int, int>(5, 40));

	// Getting memory locations
	auto new_insert_return = myHash.insert(pair<int, int>(4, 40));
	std::cout << "Key value 4: (" << new_insert_return.first << ", " << new_insert_return.second << ")\n";

	new_insert_return = myHash.insert(pair<int, int>(6, 60));
	std::cout << "Key value 6: (" << new_insert_return.first << ", " << new_insert_return.second << ")\n";

	new_insert_return = myHash.insert(pair<int, int>(5, 40));
	std::cout << "Key value 5: (" << new_insert_return.first << ", " << new_insert_return.second << ")\n";

	std::cout << "## BEGINNING REHASH TESTS ##\n";
	std::cout << "Invoking rehash(107) on myHash, previously size 101.\n";
	myHash.rehash(107);

	new_insert_return = myHash.insert(pair<int, int>(4, 40));
	std::cout << "Key value 4: (" << new_insert_return.first << ", " << new_insert_return.second << ")\n";

	new_insert_return = myHash.insert(pair<int, int>(6, 60));
	std::cout << "Key value 6: (" << new_insert_return.first << ", " << new_insert_return.second << ")\n";

	new_insert_return = myHash.insert(pair<int, int>(5, 40));
	std::cout << "Key value 5: (" << new_insert_return.first << ", " << new_insert_return.second << ")\n";

	std::cout << "Memory addresses will be changed if rehash is successful!\n";
	std::cout << "\n\t## That is the end of my tests :) moving onto original tests. ##\n\n";

	// NESTERENKO ORIGINAL TESTS

	// x will have type hashmap<int, int>::value_type*
	auto x = myHash.find(4);
	if (x != nullptr)
		cout << "4 maps to " << x->second << endl;
	else
		cout << "cannot find 4 in map" << endl;


	myHash.erase(4);

	auto x2 = myHash.find(4);
	if (x2 != nullptr)
		cout << "4 maps to " << x2->second << endl;
	else
		cout << "cannot find 4 in map" << endl;

	myHash[4] = 35;
	myHash[4] = 60;

	auto x3 = myHash.find(4);
	if (x3 != nullptr)
		cout << "4 maps to " << x3->second << endl;
	else
		cout << "cannot find 4 in map" << endl;

	hashmap<int, string> employees;

	// entering entries with indexing operator
	employees[123] = "Mike";
	employees[345] = "Charlie";
	employees[192] = "Joe";
	employees[752] = "Paul";
	employees[328] = "Peter";

	cout << "Enter new employee number: "; int num; cin >> num;
	cout << "Enter new employee name: "; string name; cin >> name;

	employees[num] = name; // "unsafe" insert

						   // searching map
	cout << "Enter employee number to look for: "; cin >> num;
	auto it = employees.find(num);
	if (it != nullptr)
		cout << it->first << ":" << it->second << endl;
	else
		cout << "employee not found" << endl;

	// removing from a map
	cout << "Enter employee number to fire: "; cin >> num;
	employees.erase(num);
	// check if not there
	auto removed = employees.find(num);
	if (removed == nullptr)
		cout << "Employee removed successfully" << endl;

}
