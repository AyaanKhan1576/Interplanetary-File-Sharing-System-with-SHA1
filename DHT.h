#include "Tree.h"
#include<cstring>
#include "sha1.hpp"
//#include <iostream>
//using namespace std;

template<typename Type, typename IdentifierType>
struct Node {
	string mech;    // Actual machine ID
	string data;    // Successor of the machine ID
	IdentifierType val;     // Value, often used to represent the identifier space or index
	void* ptr;              // Pointer to the successor's machine

	Node* next;
	Node* prev;

	Node() { next = prev = NULL; data = mech = val = 0; }
};


template<typename Type, typename IdentifierType>
class Routing_Table { // Doubly linked list
public:
	Node<Type, IdentifierType>* head;  // Pointer to the head of the linked list
	IdentifierType count;               // Count of nodes in the linked list

	// Default constructor initializing members
	Routing_Table() {
		head = NULL;
		count = 0;
	}

	// Insert a new node into the routing table
	void insert(IdentifierType id, IdentifierType Succerssor, IdentifierType secondCol, void* p = NULL) {
		Node<Type, IdentifierType>* newNode = new Node<Type, IdentifierType>;
		newNode->val = id;
		newNode->data = to_string(Succerssor);
		newNode->mech = to_string(secondCol);
		newNode->ptr = p;

		if (!head) {
			// If the linked list is empty, create a new node and set it as the head
			head = newNode;
		}
		else {
			// If the linked list is not empty, iterate to the end and append a new node
			Node<Type, IdentifierType>* temp = head;
			while (temp->next != NULL) {
				temp = temp->next;
			}

			temp->next = newNode;
			newNode->prev = temp;
		}
		count++;
	}


	// Display the contents of the routing table
	// Display the contents of the routing table
	void display() {

		Node<Type, IdentifierType>* temp = head;

		// Loop through each node in the linked list
		while (temp != NULL) {
			// Display details of the current node
			//cout << "Node Table:\t";
			cout << "  - (i): " << temp->val << "\t";

			cout << " +: " << temp->mech << "\t";
			cout << "  - SuccessorTOThisMachine: " << temp->data << "\t";
			//cout << "  - PtrToSucc: " << temp->ptr << "\t";

			// Move to the next node
			temp = temp->next;

			// Output a separator line between nodes
			if (temp != NULL) {
				cout << "\n";
			}
		}
	}


	// Delete a specific node from the routing table
	bool Delete(Node<Type, IdentifierType>* n) {
		Node<Type, IdentifierType>* temp = head;
		// Check if the head needs to be deleted
		if (head && head->data == n->data) {
			head = head->next;
			if (head)
				head->prev = NULL;

			delete temp;
			count--;
			return true;
		}
		else {
			while (temp != NULL) {
				if (temp->data == n->data) {
					Node<Type, IdentifierType>* temp1 = temp->prev;
					temp1->next = temp->next;

					if (temp->next)
						temp->next->prev = temp1;

					delete temp;
					count--;
					return true;
				}
				temp = temp->next;
			}
		}
		return false;
	}

};

template<typename Type, typename IdentifierType>
class Machinee {

public:
	string id;
	string hashed_id;
	Routing_Table<Type, IdentifierType> Fht;
	BTree Tree;

	Machinee* next;

	IdentifierType line;
	IdentifierType file_no;

	Machinee() { next = NULL; file_no = 0; line = 0; }

	Type File(Type key)
	{
		Type name = "";

		if (line == 100 || line == 0) {
			file_no++;
			line = 1;
			name = "Treefile_Machine_" + id + "_(" + to_string(file_no) + ").txt";
			ofstream file(name);
			file << key << endl;
			file.close();
		}
		else {

			ofstream file;
			name = "Treefile_Machine_" + id + "_(" + to_string(file_no) + ").txt";
			file.open(name, std::ios::app);
			file << key << endl;
			file.close();
			line++;
		}
		return name;
	}

	bool StoreKeyInMachineTree(DATA var) {
		// Generate file name and update data path and line number
		Type filePath = File(var.data + "\t\t" + var.key);
		var.path = filePath;
		var.line_no = line;

		// Insert the data into the machine's B-tree
		Tree.insert_tree(Tree, var.key, var);
		return true;
	}

};

template<typename Type, typename IdentifierType>
class Ring_DHT {


public:

	//================ RING ====================

	Machinee<Type, IdentifierType>* head;

	IdentifierType MachineCount = 0;
	IdentifierType idSpace;
	IdentifierType MaxID;



	Ring_DHT(IdentifierType Lo = 0, IdentifierType ma = 0) { head = NULL; idSpace = Lo; MaxID = ma; }

	// Add data to a machine
	//bool AddDataToMachine(DATA newData) {
	//	// Find the appropriate machine to add the data
	//	Machinee<Type, IdentifierType>* targetMachine = FindMachineForData(newData);

	//	// Add the data to the machine
	//	targetMachine->StoreKeyInMachineTree(newData);

	//	return true;
	//}

	bool AddDataToMachine(DATA newData) {
		// Check if the data should be added to the head machine
		if (head->id == newData.MachineIdinDATA) {
			head->StoreKeyInMachineTree(newData);
			return true;
		}
		else {
			// Find the appropriate machine to add the data
			Machinee<Type, IdentifierType>* currentMachine = head;

			while (currentMachine->id < newData.MachineIdinDATA && currentMachine->next != head)
				currentMachine = currentMachine->next;

			// Add the data to the machine
			if (currentMachine->id >= newData.MachineIdinDATA)
				currentMachine->StoreKeyInMachineTree(newData);
			else
				head->StoreKeyInMachineTree(newData);

			return true;
		}
	}

	Machinee<Type, IdentifierType>* FindMachineForData(const DATA& newData) {
		// Check if the data should be added to the head machine
		if (head->id == newData.MachineIdinDATA) {
			return head;
		}
		else {
			// Find the appropriate machine to add the data
			Machinee<Type, IdentifierType>* currentMachine = head;

			while (currentMachine->id < newData.MachineIdinDATA && currentMachine->next != head)
				currentMachine = currentMachine->next;

			// Return the machine where the data should be added
			return (currentMachine->id >= newData.MachineIdinDATA) ? currentMachine : head;
		}
	}


	void Add_Machine(string idd, string hid) {

		Machinee<Type, IdentifierType>* newMachine = new Machinee < Type, IdentifierType >;

		newMachine->id = idd;	newMachine->hashed_id = hid;
		//Machinee<Type, IdentifierType>* temp = insert(idd, hid);

		Machinee<Type, IdentifierType>* insertPosition = head;

		while (insertPosition->id < idd)
		{						//9
			insertPosition = insertPosition->next;
			if (insertPosition == head)
				break;
		}

		BTreeNode* node = insertPosition->Tree.root;
		insertPosition->Tree.root = NULL;
		insertPosition->file_no = NULL;
		insertPosition->line = NULL;

		if (insertPosition != head)
		{

			insertPosition = head;

			while (insertPosition->next->id < idd)
			{
				insertPosition = insertPosition->next;	//5
				if (insertPosition->next == head)
					break;
			}

			newMachine->next = insertPosition->next;
			insertPosition->next = newMachine;


			MachineCount++;

			insertPosition = head;

			for (int i = 0; i < MachineCount; i++)
			{
				insertPosition->Fht.head = NULL;
				insertPosition = insertPosition->next;
			}
		}
		else {

			while (insertPosition->next != head)
				insertPosition = insertPosition->next;

			newMachine->next = head;
			insertPosition->next = newMachine;
			head = newMachine;
			if (head->id > head->next->id)head = head->next;
			MachineCount++;
			insertPosition = head;

			for (int i = 0; i < MachineCount; i++)
			{
				insertPosition->Fht.head = NULL;
				insertPosition = insertPosition->next;
			}
		}
		go(node);

	}

	//print problem might be here 
	//template<typename Type, typename IdentifierType>
	void go(BTreeNode* node, IdentifierType value = 0) {
		if (node == nullptr)
			return;

		int i = 0;
		if (value <= 0)
			AddDataToMachine(node->objects[i++]);  // Preorder

		for (int i = 0; i < node->numKeys + 1; ++i) {
			if (node->children[i] != nullptr)
				node->displayNode(node->children[i], value);  // LVR display
			if (value == 1 && i < node->numKeys)
				AddDataToMachine(node->objects[i++]);  // Inorder
		}

		if (value > 1)
			AddDataToMachine(node->objects[i++]);  // Postorder
	}


	void delete_machine(string idd) {
		Machinee<Type, IdentifierType>* temp1 = head;

		// Find the machine with the specified ID
		if (!find_machine_by_id(temp1, idd)) {
			cout << "This ID doesn't exist\n";
			return;
		}

		// Extract the logic to delete a machine into a separate function
		BTreeNode* node = delete_machine_logic(temp1);

		// Update the routing tables and display the information
		update_routing_tables_and_display(node);
	}
	//==========HELPER FUNCTIONS FOR DELETE_MACHINE=================================
	bool find_machine_by_id(Machinee<Type, IdentifierType>*& result, const string& idd) {
		result = head;
		while (result->id != idd) {
			result = result->next;

			if (result == head) {
				return false;
			}
		}
		return true;
	}

	void delete_non_head_machine(Machinee<Type, IdentifierType>* temp1) {
		Machinee<Type, IdentifierType>* temp = head;
		while (temp->next->id != temp1->id) {
			temp = temp->next;
		}

		temp->next = temp1->next;
		delete temp1;

		MachineCount--;

		temp1 = head;

		for (int i = 0; i < MachineCount; i++) {
			temp1->Fht.head = NULL;
			temp1 = temp1->next;
		}
	}

	void delete_head_machine(Machinee<Type, IdentifierType>* temp1) {
		Machinee<Type, IdentifierType>* temp = head;
		while (temp->next != head) {
			temp = temp->next;
		}

		temp->next = head->next;
		delete head;
		head = temp->next;
		temp1 = head;
		MachineCount--;

		for (int i = 0; i < MachineCount; i++) {
			temp1->Fht.head = NULL;
			temp1 = temp1->next;
		}
	}
	BTreeNode* delete_machine_logic(Machinee<Type, IdentifierType>* temp1) {
		BTreeNode* node = temp1->Tree.root;
		temp1->Tree.root = NULL;
		temp1->file_no = NULL;
		temp1->line = NULL;

		if (temp1 != head) {
			delete_non_head_machine(temp1);
		}
		else {
			delete_head_machine(temp1);
		}

		return node;
	}
	void update_routing_tables_and_display(BTreeNode* node) {
		go(node);
	}
	//==========HELPER FUNCTIONS FOR DELETE_MACHINE END=================================

	// Insert a Machine Node
	Machinee<Type, IdentifierType>* insert(string idd, string hid) {
		// Check if the linked list is empty
		if (!head) {
			// Create a new Machinee node as the head
			head = new Machinee<Type, IdentifierType>;
			head->id = idd;
			head->hashed_id = hid;
			head->next = head; // Circular linked list
			MachineCount++;
			return NULL;
		}
		else if (head->id > idd) {
			// Insert before the current head
			return insert_before_head(idd, hid);
		}
		else {
			// Insert in the appropriate position
			return insert_in_position(idd, hid);
		}
	}
	//==========HELPER FUNCTIONS FOR INSERT=================================
	Machinee<Type, IdentifierType>* insert_before_head(string idd, string hid) {
		Machinee<Type, IdentifierType>* temp = head;
		// Traverse to the last node in the circular linked list
		while (temp->next != head)
			temp = temp->next;

		// Create a new Machinee node and insert it after the last node
		temp->next = new Machinee<Type, IdentifierType>;
		temp->next->id = idd;
		temp->next->hashed_id = hid;
		temp->next->next = head;

		// Update the head to the newly inserted node
		if (head->next == head)
			head->next = temp->next;

		head = temp->next;
		MachineCount++;
		return temp;
	}

	Machinee<Type, IdentifierType>* insert_in_position(string idd, string hid) {
		Machinee<Type, IdentifierType>* temp = head;
		while (temp->next != head) {
			// Check for duplicate machine IDs
			if (temp->id == idd || temp->next->id == idd) {
				cout << "\nCannot Enter Machine. It's already Present!";
				return NULL;
			}
			// Break if the next node has a greater ID
			if (temp->next->id > idd)
				break;
			else
				temp = temp->next;
		}

		// Insert the new machine node
		Machinee<Type, IdentifierType>* tempo = temp->next;
		temp->next = new Machinee<Type, IdentifierType>;
		temp->next->id = idd;
		temp->next->hashed_id = hid;
		temp->next->next = tempo;
		MachineCount++;
		return temp;
	}
	//==========HELPER FUNCTIONS FOR INSERT END=================================


	void Ret(Machinee<Type, IdentifierType>* temp, string key) {	//Find and Show Data

		if (!temp) return;
		cout << "\nMachine NO. " << temp->id << endl;
		//BTreeNode* lo = temp->Tree.retrieve(key);
		BTreeNode* lo = temp->Tree.searchBTree2(temp->Tree.root, key);
		if (!lo) { cout << "\nNULL\n"; return; }

		int i = 0;
		while (i <= lo->numKeys)
		{
			cout << endl << endl;
			cout << "=======================Data==============================" << endl;
			cout << "data\t" << lo->objects[i].data << endl;	// left root right
			cout << "path\t" << lo->objects[i].path << endl;	// left root right
			cout << "Line no\t" << lo->objects[i].line_no << '\n';	// left root right
			cout << "mach\t" << lo->objects[i].MachineIdinDATA << endl;;	// left root right
			cout << "key\t" << lo->objects[i].key << endl;	// left root right
			cout << "By File\n";

			ifstream input;
			int numOfInps = 0;
			Type data;
			input.open(lo->objects[i].path);
			//cout << temp->line << endl;
			for (int i = 1; (/*(!input.eof()) ||*/ i <= temp->line); i++)
			{
				//exception here
				getline(input, data);
				if (i == lo->objects[i].line_no) { cout << "\n data = " << data; break; }
			}
			input.close();
			i++;
		}
	}

	IdentifierType Successor(IdentifierType log) {

		if (stoi(head->id) == log)
			return stoi(head->id);
		else {

			Machinee<Type, IdentifierType>* temp = head;

			while (stoi(temp->id) < log && temp->next != head)
				temp = temp->next;
			if (stoi(temp->id) >= log)
				return stoi(temp->id);
			else
				return stoi(head->id);
		}
	}

	void Search(string key, IdentifierType mac = 0) {
		string log = key;
		log = Successor(log);
		Machinee<Type, IdentifierType>* temp = head;
		Node<Type, IdentifierType>* tempo = temp->Fht.head;

		while (1) {
			tempo = temp->Fht.head;
			cout << "\n\nMachine no =  " << temp->id;
			int i = 0;

			if (log == temp->hashed_id) {
				cout << "\n=====1=======\n";
				Ret(temp, key);
				return;
			}
			else if (temp->hashed_id < log && log <= tempo->data) {
				//temp = static_cast<Machinee<Type, IdentifierType>*>(tempo->ptr); cout << "\n=====2=======\n"; Ret(temp, key); return;
			}
			else {
				while (tempo) {
					if (tempo->next != NULL) {
						if (tempo->data < log && log <= tempo->next->data) {
							cout << "\n=====3=======\n";
							temp = static_cast<Machinee<Type, IdentifierType>*>(tempo->ptr);
							break;
						}
					}

					if (tempo->next == NULL) {
						cout << "\n=====4=======\n";
						temp = static_cast<Machinee<Type, IdentifierType>*>(tempo->ptr);
						break;
					}

					cout << "\ntempo\t" << i << "\n";
					i++;
					tempo = tempo->next;
				}
			}
		}
	}



	// Set up the finger table for each machine in the ring
	void Set(string* machineIDs, int currentMachineID, IdentifierType totalMachinesInRing) {
		Machinee<Type, IdentifierType>* currentMachine = head;

		// Loop through each machine in the ring
		for (int currentMachineIndex = 0; currentMachineIndex < MachineCount; currentMachineIndex++) {
			// For each machine, calculate the finger table entries
			for (int fingerIndex = 1; fingerIndex <= currentMachineID; fingerIndex++) {
				/*// Calculate the hashed ID for the finger
				SHA1 Hash1;
				Hash1.update(currentMachine->id + to_string(pow(2, fingerIndex - 1)));
				string hashedID = Hash1.final();*/
				IdentifierType SecondColoumn = (IdentifierType(stoi(currentMachine->id) + pow(2, fingerIndex - 1))) % IdentifierType(pow(2, currentMachineID));


				// Insert the finger table entry
				currentMachine->Fht.insert(fingerIndex, Successor(SecondColoumn), SecondColoumn, Get_ptr(Successor(SecondColoumn)));
			}

			// Move to the next machine in the ring
			currentMachine = currentMachine->next;
		}
	}

	void sortMachineArray(string* machineArray, int totalMachines) {
		string temp;

		for (int i = 0; i < totalMachines; i++) {
			for (int j = i + 1; j < totalMachines; j++) {
				if (machineArray[i] > machineArray[j]) {
					temp = machineArray[i];
					machineArray[i] = machineArray[j];
					machineArray[j] = temp;
				}
			}
		}
	}

	string* updateMachineArray(int totalMachines, string* machineArray, const string& machineIdInput) {
		string* updatedMachineArray = new string[totalMachines + 1];

		for (int i = 0; i < totalMachines; i++)
			updatedMachineArray[i] = machineArray[i];

		updatedMachineArray[totalMachines] = machineIdInput;

		return updatedMachineArray;
	}

	// Display information about all machines in the ring
	void display() {
		// Check if the linked list is empty
		if (!head) {
			cout << "No machines in the ring.\n";
			return;
		}

		Machinee<Type, IdentifierType>* temp = head;

		// Loop through all machines in the circular linked list
		do {
			cout << "\n----------------------------------------------------------\n";
			// Display the information for the current machine
			displayMachineInfo(temp);

			// Move to the next machine in the circular linked list
			temp = temp->next;
		} while (temp != head);  // Loop until we reach the initial machine
		cout << "\n----------------------------------------------------------\n";
		cout << endl;
	}

	// Display information for a single machine
	void displayMachineInfo(Machinee<Type, IdentifierType>* machine) {

		cout << "\nMachine id: " << machine->id << "\tMachine's HashedID; " << machine->hashed_id << endl;
		//cout <<"Adress Of Machine: " << machine << endl;
		//machine->Tree.display(machine->Tree.root, machine->id);
		machine->Tree.display(machine->Tree.root, 0);


	}


	Machinee<Type, IdentifierType>* Get_ptr(IdentifierType str) {

		Machinee<Type, IdentifierType>* temp = head;
		bool b = 1;
		while (temp != head || b) {
			b = 0;
			if (stoi(temp->id) == str) return temp;
			temp = temp->next;
		}
		//return ;	
	}



};