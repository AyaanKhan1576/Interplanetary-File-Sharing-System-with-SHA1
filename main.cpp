#include <iostream>
#include <string>
#include <sstream>
#include "DHT.h"
#include "sha1.hpp"
using namespace std;

int insertedMachines = 0;

// Function to perform modulo operation on machine IDs
string computeMachineId(string value, string max) {
	return to_string(stoi(value) % stoi(max));
}


//======================= menu ======================= 


template<class KeyType, class ValueType>
bool inputDataInRing(int totalMachines, int IDS_Power, Ring_DHT<KeyType, ValueType>& DHTmain) {
	string inputData;
	bool success = false;

	int Total_IDS_bits = pow(2, IDS_Power);
	do {
		// Prompt user to enter the data
		cout << "\nEnter the data you want to store: ";
		getline(cin, inputData);

		if (!inputData.empty()) {
			DATA dataObject;
			SHA1 hashFunction;

			// Set the data and compute the hash
			dataObject.data = inputData;
			hashFunction.update(inputData);
			dataObject.key = hashFunction.final();
			// Assuming data.key is a hexadecimal string
				//changes
				/*std::string hexSubstring = dataObject.key.substr(0, 8); // Extract the first 4 characters
				int intValue;

				std::istringstream(hexSubstring) >> std::hex >> intValue;

				dataObject.MachineIdinDATA = intValue % Total_IDS_bits;*/

				// Add data to the distributed hash table
			dataObject.MachineIdinDATA = inputData;
			DHTmain.AddDataToMachine(dataObject);

			// Indicate successful data input
			cout << "Data successfully added to the distributed hash table.\n";
			success = true;
		}
		else {
			cout << "Invalid input. Data cannot be empty. Please try again.\n";
		}

	} while (!success);

	return success;
}

template<class KeyType, class ValueType>
void searchDataInDHT(Ring_DHT<KeyType, ValueType>& DHT1) {
	string searchInput = " ";
	cout << "\nEnter the search term:\n";
	getline(cin, searchInput);

	if (searchInput != "-1") {
		SHA1 Hash;
		string hashedString;
		Hash.update(hashedString);
		DHT1.Search(Hash.final());
	}
}

// Function to add a new machine dynamically to the distributed hash table
template<class KeyType, class ValueType>
bool DynamicallyAddMachine(int totalMachines, string* machineArray,Ring_DHT<KeyType, ValueType>& distributedHashTable, int IDS_power) {
	string newMachineId ;

	// Prompt user to enter the machine ID
	do {
		cout << "Enter the ID for the new machine: ";
		getline(cin, newMachineId);

		if (newMachineId.empty()) {
			cout << "Invalid input. Machine ID cannot be empty. Please try again.\n";
		}
	} while (newMachineId.empty());

	// Hash the machine ID
	SHA1 hashFunction;
	hashFunction.update(newMachineId);
	string hashedId = hashFunction.final();

	// Add the new machine to the distributed hash table
	distributedHashTable.Add_Machine(newMachineId, hashFunction.final());

	// Update the machine array and set it in the distributed hash table
	string* updatedMachineArray = distributedHashTable.updateMachineArray(totalMachines, machineArray, newMachineId);
	distributedHashTable.sortMachineArray(updatedMachineArray, totalMachines);
	distributedHashTable.Set(updatedMachineArray, IDS_power, insertedMachines);

	// Indicate successful addition of the new machine
	cout << "New machine successfully added to the distributed hash table.\n";

	return true;
}



// Function to remove a machine from the distributed hash table
template<class KeyType, class ValueType>
bool removeMachineFromDHT(int totalMachines, string* machineArray, Ring_DHT<KeyType, ValueType>& distributedHashTable, int IDS_power) {
	string machineIdInput = "";

	// Prompt user to enter the ID of the machine to be deleted
	do {
		cout << "Enter the ID of the machine to be deleted: ";
		getline(cin, machineIdInput);

		if (machineIdInput.empty()) {
			cout << "Invalid input. Machine ID cannot be empty. Please try again.\n";
		}
	} while (machineIdInput.empty());

	// Remove the specified machine from the distributed hash table
	distributedHashTable.delete_machine(machineIdInput);

	// Use insertion sort to sort the array
	for (int i = 1; i < totalMachines - 1; ++i) {
		string key = machineArray[i];
		int j = i - 1;

		// Move elements of machineArray[0..i-1] that are greater than key to one position ahead of their current position
		while (j >= 0 && machineArray[j] > key) {
			machineArray[j + 1] = machineArray[j];
			--j;
		}

		machineArray[j + 1] = key;
	}

	// Other logic can be added here...

	insertedMachines--;
	distributedHashTable.Set(machineArray, IDS_power, insertedMachines);
	return true;
}


template<class T, class U>
void printBTree(Ring_DHT<T, U>& DHT1, string MachineID/*machineidtoprint*/)
{
	Machinee<T, U>* temp = DHT1.head;
	bool check = true;
	//check if ID exists in the ringDHt
	while (temp->id != MachineID) {
		temp = temp->next;
		if (temp == NULL) {
			cout << "id not found\n";
			check = false;
			break;
		}
	}
	if (check)
		temp->Tree.display(temp->Tree.root, 0);
}
template<class T, class U>
void printroutingtabble(Ring_DHT<T, U>& DHT1)
{
	string machineIdInput;
	do {
		cout << "Machine ID for which routing table to be made : ";
		getline(cin, machineIdInput);

		if (machineIdInput.empty()) {
			cout << "Invalid input. Machine ID cannot be empty. Please try again.\n";
		}
	} while (machineIdInput.empty());

	Machinee<T, U>* temp = DHT1.head;
	bool machineFound = false;

	for (; temp != nullptr; temp = temp->next) {
		if (temp->id == machineIdInput) {
			machineFound = true;
			break;
		}
	}

	if (machineFound) {
		temp->Fht.display();
	}
	else {
		cout << "Machine ID not found.\n";
	}
}


// Function to remove data from the distributed hash table
template<class T, class U>
bool removeDataFromRing(Ring_DHT<T, U>& distributedHashTable) {
	string dataToRemove;

	// Prompt user to enter the data for removal
	do {
		cout << "Enter the data you want to be removed: ";
		getline(cin, dataToRemove);

		if (dataToRemove.empty()) {
			cout << "Invalid input. Data cannot be empty. Please try again.\n";
		}
	} while (dataToRemove.empty());

	bool dataFound = false;
	Machinee<T, U>* currentMachine = distributedHashTable.head;
	SHA1 hashFunction;

	while (currentMachine) {
		BTreeNode* dataNode = currentMachine->Tree.searchBTree2(currentMachine->Tree.root, dataToRemove);

		if (dataNode) {
			// Data found, proceed with removal
			dataFound = true;
			hashFunction.update(dataToRemove);
			string hashedData = hashFunction.final();
			currentMachine->Tree.Delete(currentMachine->Tree.root, dataToRemove);
			break;
		}

		currentMachine = currentMachine->next;

		if (currentMachine == distributedHashTable.head)
			break;
	}

	if (dataFound) {
		cout << "Data successfully removed from the distributed hash table.\n";
		return true;
	}
	else {
		cout << "Data not present. Unable to remove.\n";
		return false;
	}
}

template <class KeyType, class ValueType>
void mainMenu(int totalMachines, string* machineArray, int IDS_power, Ring_DHT<KeyType, ValueType>& DHTmain) {
	int FilesInputed = 0;
	int Total_IDS_bits = pow(2, IDS_power);

	string userInput = "";

	while (userInput != "-1") {
		cout << "\n\t\tMAIN MENU\n";
		cout << "1. Add new Data\n";
		cout << "2. Delete file using its hash key\n";
		cout << "3. Display B-tree for a specific machine and its stored files\n";
		cout << "4. Show routing table for a particular machine\n";
		cout << "5. Add a new machine to the DHT\n";
		cout << "6. Remove an existing machine from the DHT\n";
		getline(cin, userInput);

		switch (stoi(userInput)) {
		case 1:
			if (FilesInputed >= Total_IDS_bits) {
				cout << "Maximum IDS Holding Capacity Reached. Unable to Enter More Data.\n";
			}
			else {

				bool DataInputted = inputDataInRing(totalMachines, IDS_power, DHTmain);
				//print all machines with data added 
				if (DataInputted) {
					FilesInputed++;
					DHTmain.display();
				}

			}
			break;

		case 2:
			if (FilesInputed <= 0) {
				cout << "No Data Present.\n";
			}
			else {
				if (removeDataFromRing(DHTmain))
					FilesInputed--;

				DHTmain.display();
			}
			break;

		case 3:
		{
			string machineIdInput;
			do {
				cout << "Machine ID for which BTREE to be made : ";
				getline(cin, machineIdInput);

				if (machineIdInput.empty()) {
					cout << "Invalid input. Machine ID cannot be empty. Please try again.\n";
				}
			} while (machineIdInput.empty());

			printBTree(DHTmain, machineIdInput);
			break;
		}

		break;

		case 4:
		{
			
			printroutingtabble(DHTmain);
			break;
		}

		break;

		case 5:
			if (DHTmain.MachineCount >= DHTmain.MaxID) {
				cout << "Maximum Machine Holding Capacity Reached for this IDS.\n";
			}
			else {
				bool done=  DynamicallyAddMachine(totalMachines, machineArray,DHTmain , IDS_power);
				if (done)DHTmain.display();
			}
			break;

		case 6:
			cout << "Machine Count: " << DHTmain.MachineCount << endl;
			if (DHTmain.MachineCount == 0) {
				cout << "No Machines in the Ring.\n";
			}
			else {
				cout << ".";
				bool done = removeMachineFromDHT( totalMachines, machineArray, DHTmain,IDS_power);
				if (done)DHTmain.display();
			}
			break;



		default:
			cout << "Invalid input. Please enter a valid option.\n";
		}
	}
}


string genRandomstring()
{
	string str;
	srand(time(0));
	const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	int stringLength = sizeof(alphanum) - 1;
	for (int k = 0; k < 7; k++) {
		str += alphanum[rand() % stringLength];
	}
	return str;
}

#include <iomanip>
int main() {
	srand(time(0));
	// START UP MESSAGE
	std::cout << "+----------------------------------------------------+\n";
	std::cout << "| Welcome to the Distributed Hash Table (DHT) System |\n";
	std::cout << "+----------------------------------------------------+\n";

	int manualAssignment = 0;
	string manualAssignmentInput;
	int initialMachineCount = 0;
	string* machineIds;
	string userInput;
	string IDS;
	int IDS_power = 0;
	
	int Total_IDS_bits = 0;
	int insertedMachines = 0; // Added variable to track the number of inserted machines

	
	//long long unsigned int a = 1;
	//string b;
	//std::stringstream why;
	//SHA1 test;
	//test.update(to_string(a));
	//b = test.final();
	//why << std::hex << b;
	//why >> a;
	//cout << b << endl;
	//cout << a << endl;
	//why.clear();
	//why.str("");
	//why << std::hex << a;
	//b = why.str();
	//cout << b;



	do {
		cout << "Enter identifier space: ";
		getline(cin, IDS);
		IDS_power = stoi(IDS);
		Total_IDS_bits = pow(2, IDS_power);
		machineIds = new string[Total_IDS_bits];

		cout << "Enter number of Machines initially: ";
		getline(cin, userInput);
		initialMachineCount = stoi(userInput);

		if (Total_IDS_bits < initialMachineCount) {
			cout << "Error !! Number of Machines Can't Exceed Total IDS bits \nRe-enter " << endl;
		}

	} while (initialMachineCount > Total_IDS_bits);



	cout << "\nIf you want to assign machine IDs manually, PRESS 1 and enter, else Press 2\n";
	getline(cin, manualAssignmentInput);
	manualAssignment = stoi(manualAssignmentInput);//convert string to int 




	bool isUnique = true;
	string tempId = "";
	int j = 0;
	int count = 1;

	// Automatically assign a value if the user doesn't input manually
	if (manualAssignment != true) {
		int i = 0;
		while (i < initialMachineCount) {
			// Generate a random machine ID within the specified range
			//tempId = 0 + rand() % (Total_IDS_bits - 1);
			//tempId = genRandomstring();
			tempId = to_string(rand() % Total_IDS_bits + 1);

			isUnique = true;
			int j = 0;

			// Check if the generated ID is unique among existing machine IDs
			while (j < i) {
				if (tempId == machineIds[j])
					isUnique = false;
				j++;
			}

			// If the generated ID is unique, assign it to the array
			if (isUnique) {
				machineIds[i] = tempId;
				i++;
			}
		}
	}
	// If user chooses manual input
	else {
		// Get the number of machines from user input

		for (int i = 0; i < initialMachineCount;) {
			// Prompt user to enter machine ID
			for (;;) {
				cout << "\nEnter machine ID to store: ";
				getline(cin, userInput);

				if (userInput.empty()) {
					cout << "Invalid input. Machine ID cannot be empty. Please try again.\n";
				}
				else {
					break;
				}
			}

			tempId = userInput;

			// Handle the case where the entered ID is larger than the specified range
			for (; stoi(tempId) >= Total_IDS_bits;) {
				cout << "The entered machine ID is larger than the specified range. Please enter a valid machine ID.\n";
				// Prompt user to re-enter machine ID
				for (;;) {
					cout << "ReEnter machine ID to store: ";
					getline(cin, userInput);

					if (userInput.empty()) {
						cout << "Invalid input. Machine ID cannot be empty. Please try again.\n";
					}
					else {
						break;
					}
				}

				tempId = userInput;
			}

			isUnique = true;
			int j = 0;

			// Check if the entered ID is unique among existing machine IDs
			while (j < i) {
				if (tempId == machineIds[j])
					isUnique = false;
				j++;
			}

			// If the entered ID is unique, assign it to the array
			if (isUnique) {
				insertedMachines++;
				machineIds[i] = tempId;
				i++;
			}
		}
	}


	// Insertion Sort for the array of machine IDs
	for (int i = 1; i < initialMachineCount; i++) {
		string key = machineIds[i];
		int j = i - 1;

		// Move elements of the array that are greater than key to one position ahead of their current position
		while (j >= 0 && stoi(machineIds[j]) > stoi(key)) {
			machineIds[j + 1] = machineIds[j];
			j = j - 1;
		}

		machineIds[j + 1] = key;
	}


	// Display the sorted machine IDs
	cout << "\nSorted MACHINEID BELOW\n";

	// Create a Ring_DHT object
	SHA1 Hash;
	stringstream ss;

	Ring_DHT<string, int> DHTmain(IDS_power, Total_IDS_bits);

	string hashString;
	//Hash the Machines IDs and then insert to Ring
	int i = 0;
	while (i < initialMachineCount) {
		Hash.update(machineIds[i]);
		hashString = Hash.final();
		cout << "Machine ID:  " << machineIds[i] << "\t\tHashed Id:  " << hashString << endl;

		DHTmain.insert(machineIds[i], hashString);

		i++;
	}

	cout << "MachineCount=" << DHTmain.MachineCount << endl;
	bool go = true;
	if (go) {
		// Set the machine IDs and identifier space in the distributed hash table
		DHTmain.Set(machineIds, IDS_power, insertedMachines);

		// Call the main menu function
		mainMenu(Total_IDS_bits, machineIds, IDS_power, DHTmain);
		go = false;
	}
	

	// Deallocate memory for the machine IDs array
	delete[] machineIds;

	return 0;

}