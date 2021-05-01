/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

				Group Members:
				Aleezeh Usman 18i-0529
				Laiba Imran   18i-0477
				Areesha Tahir 18i-1655

				DS FINAL PROJECT : AIRLINE RESERVATION SYSTEM
				Submission Date: 5-11-19

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#pragma once
#include<iostream>
#include<sstream>
#include<iomanip>
#include<Windows.h>
#include<conio.h>
using namespace std;
int TimeDiffWithDate(string strt_time, string end_time);

//--------------------------------------<<UTILITY DATA TYPES>>------------------------------------------------------------------
//utility data structures
template<typename T>
class Node
{
public:
	T data;
	bool visited;
	Node* next;
	Node() {
		next = NULL;
		visited = false;
	}
};

template<typename T>
class Queue {
public:
	Node<T>* head;
	Node<T>* tail;

	//constructor
	Queue() {
		head = NULL;
		tail = NULL;
	}

	//Insert and Remove
	void  enqueue(T d) {
		if (head == NULL && tail == NULL) {				//inserting into an empty list
			head = new Node<T>;
			head->data = d;
			tail = head;
		}
		else if (head != NULL) {					//not empty so head not changed only tail changed
			tail->next = new Node<T>;
			tail = tail->next;
			tail->data = d;
		}
	}
	T dequeue() {
		if (!E()) {
			Node<T>* temp = head;
			if (head == tail) {					//only one element in list
				head = tail = NULL;
				return temp->data;
			}
			else if (head != tail) {			//more than one element in list
				head = head->next;
				return temp->data;
			}
		}
		else cout << "<<<<List is empty>>>>\n";
	}

	//utility functions
	void display() {
		if (!E()) {
			Node<T>* temp = head;
			while (temp != NULL) {
				cout << temp->data;
				temp = temp->next;
			}
			cout << endl;
		}
		else cout << "<<<<List is empty>>>>\n";

	}
	bool E() {
		if (head == tail && tail == NULL)return true;
		else return false;
	}
	void C() {
		while (head != NULL) {
			dequeue();
		}
	}
	bool find(T tofind) {
		Node<T>* temp = head;
		while (temp != NULL) {
			if (temp->data == tofind) return true;
			temp = temp->next;
		}
		return false;
	}
	void deleteLocationfromQueue(T todelete) {
		if (head->data == todelete) {
			Node<T>* temp = head;
			if (head == tail) tail = head->next;
			head = head->next;
			delete temp;
		}
		else if (head->data != todelete) {
			Node<T>* temp = head;
			while (temp != NULL) {
				if (temp->next->data == todelete) {
					Node<T>* DeleteNode = temp->next;
					if (tail == temp->next && tail->next != NULL) tail = tail->next;
					else if (tail->next == NULL) tail = temp;
					temp->next = temp->next->next;
					delete DeleteNode;
					break;
				}
				temp = temp->next;
			}
		}
	}

	//operator overloading
	void operator=(Queue<T> other) {
		C();
		Node<T>* temp = other.head;
		while (temp != NULL) {
			enqueue(temp->data);
			temp = temp->next;
		}
	}
	//destructor
	~Queue() {
		C();
	}
};

template<typename T>
class stack {
	Node<T>* top;

public:
	stack() {
		top = NULL;
	}

	//insertion deletion functions
	void push(Node<T>& dt) {
		if (top == NULL) {							//in case stack was empty will not have to deal with next ptr
			top = new Node<T>;
			top->data = dt.data;
		}
		else {
			Node<T>* tempnode = top;			//in case stack not empty will have to 
			top = new Node<T>;									//alter next ptr to make it point to prev value
			top->next = tempnode;
			top->data = dt.data;
		}
	}
	Node<T> pop() {
		if (!isEmpty()) {
			Node<T> toReturn;
			toReturn.data = top->data;
			Node<T>* toDelete = top;
			top = top->next;
			delete toDelete;							//move top to the next element i.e one below it
			return toReturn;
		}
		else cout << "stack is empty :( \n";
	}

	//utility functions
	bool isEmpty() {
		if (top == NULL)return true;				//if top is empty than nothing in stack 
		else
			return false;
	}
	void clear() {
		while (!isEmpty()) pop();					//keep popping values until stack becomes empty
	}
	void display() {
		Node<T>* temp = top;
		while (temp != NULL) {
			cout << temp->data;
			temp = temp->next;
		}
	}
	bool find(T tofind) {
		Node<T>* temp = top;
		while (temp != NULL) {
			if (temp->data == tofind) {
				return true;
			}
			temp = temp->next;
		}
		return false;
	}

	~stack() {
		//clear();
	}
};



//==============================================================================================================================
//							PRIMARY DATA STRUCTURES USED AS BUILDING BLOCKS IN PROJECT
//==============================================================================================================================

class edge {
public:
	int date;
	string starttime;
	string endtime;
	string airline;
	int ticket_price;
	edge() {
		date = ticket_price = -1;
		starttime = endtime = airline = "";
	}
	bool operator==(edge other) {
		if (date != other.date) return 0;
		else if (starttime != other.starttime) return 0;
		else if (endtime != other.endtime) return 0;
		else if (airline != other.airline) return 0;
		else if (ticket_price != other.ticket_price) return 0;
		return 1;
	}
};

class vertex {
public:
	string destination;
	edge flight_details;
	vertex* next;
	vertex() {
		destination = "";
		next = NULL;
	}
	void displayvertex() {
		cout << destination << "  " << flight_details.date << "  " << flight_details.starttime << "  " << flight_details.endtime << "  "
			<< flight_details.ticket_price << "  " << flight_details.airline << endl;;
	}
	bool operator==(vertex other) {
		if (destination == other.destination && flight_details == other.flight_details) return true;
		else return false;
	}
	~vertex() {
	//	delete next;
	}
};

class Adjlist {
public:
	vertex* src_head;							//head of each source, row traversal will give neighbours of the source
	string source;								//source itself i.e city name
	Adjlist() {
		src_head = NULL;
		source = "";
	}
	void InitializeVertex(string sr) {			//to initialize or create a vertex which hasnt been created before
		if (src_head == NULL) {
			src_head = new vertex;				//initialize source vertex
			source = sr;
		}
	}
	void insertflight(string c, string date, string st, string en, int price, string airl) {
		vertex* temp = src_head;
		while (temp->next != NULL) {
			temp = temp->next;					//move to end
		}
		temp->next = new vertex;
		temp = temp->next;
		temp->destination = c;
		int d;
		stringstream(date) >> d;				//convert date given in string to integer 
		temp->flight_details.date = d;			//also since at one time all flights only of one month being handled so only need to store days rest is constant
		temp->flight_details.starttime = st;
		temp->flight_details.endtime = en;
		temp->flight_details.ticket_price = price;
		temp->flight_details.airline = airl;
	}
	void display() {
		vertex* temp = src_head;
		while (temp != NULL) {
			if (temp != src_head) {
				cout << "-->" << temp->destination << "//" << temp->flight_details.airline << "//" << temp->flight_details.date << "//" << temp->flight_details.starttime << 
					" till "<<temp->flight_details.endtime<<"//"<<temp->flight_details.ticket_price<<endl << "\t";
			}
			else if (temp == src_head)
				cout << source;
			temp = temp->next;
		}
	}

	vertex returnmincost(string dest, int date, string& strt_time, string air_line, stack<vertex> checkpaths) {
		vertex* temp = src_head->next;
		int min_cost = 999999999;							//minimum cost to be stored
		vertex toReturn;
		int curr_time;
		int flighttime;

		while (temp != NULL) {
			if (temp->destination == dest) {
				stringstream(strt_time) >> curr_time;
				stringstream(temp->flight_details.starttime) >> flighttime;
				if (temp->flight_details.ticket_price < min_cost && temp->flight_details.date == date && flighttime >= curr_time && !checkpaths.find(*temp)) {
					if (air_line == "") {
						min_cost = temp->flight_details.ticket_price;
						toReturn = *temp;
					}
					else if (air_line != "") {
						if (air_line == temp->flight_details.airline) {
							min_cost = temp->flight_details.ticket_price;
							toReturn = *temp;
						}
					}
				}
			}
			temp = temp->next;
		}
		return toReturn;
	}
	Queue<vertex> neighbours(int date, string& strt_time, string air_line, stack<vertex> checkpath) {
		if (src_head != NULL) {
			Queue<vertex> toReturn;					//will return vertixes of all direct flights from a certain vertex i.e all neighbours in graph
			vertex* temp = src_head->next;
			Queue<string> check;					//to keep a check on whether current city's minimum price vertex has been enques before
			while (temp != NULL) {
				if (!check.find(temp->destination)) {
					vertex store = returnmincost(temp->destination, date, strt_time, air_line,checkpath);			//find flight with minimum cost
					if (store.destination != "") {
						toReturn.enqueue(store);
						check.enqueue(temp->destination);
					}
				}
				temp = temp->next;
			}
			return toReturn;
		}
	}

	vertex returnmintime(string dest, int date, string& strt_time, string air_line, stack<vertex> checkpaths) {
		vertex* temp = src_head->next;
		int min_time = 999999999;							//minimum time to be stored but begin with an illogically large value to replace later on after comaprison
		vertex toReturn;
		int flight_time;
		int flighttime;
		int curr_time;

		while (temp != NULL) {
			if (temp->destination == dest) {
				stringstream(strt_time) >> curr_time;
				stringstream(temp->flight_details.starttime) >> flighttime;
				flight_time = TimeDiffWithDate(temp->flight_details.starttime, temp->flight_details.endtime);
				if (flight_time < min_time && temp->flight_details.date == date && flighttime >= curr_time && !checkpaths.find(*temp)) {
					if (air_line == "") {
						min_time = flight_time;
						toReturn = *temp;
					}
					else if (air_line != "") {
						if (air_line == temp->flight_details.airline) {
							min_time = flight_time;
							toReturn = *temp;
						}
					}
				}
			}
			temp = temp->next;
		}
		return toReturn;
	}
	Queue<vertex> neighboursT(int date, string& strt_time, string air_line, stack<vertex> checkpaths) {
		if (src_head != NULL) {
			Queue<vertex> toReturn;					//will return vertixes of all direct flights from a certain vertex i.e all neighbours in graph
			vertex* temp = src_head->next;
			Queue<string> check;					//to keep a check on whether current city's minimum price vertex has been enques before
			while (temp != NULL) {
				if (!check.find(temp->destination)) {
					vertex store = returnmintime(temp->destination, date, strt_time, air_line, checkpaths);		//find flight with minimum time
					if (store.destination != "") {
						toReturn.enqueue(store);
						check.enqueue(temp->destination);
					}
				}
				temp = temp->next;
			}
			return toReturn;
		}
	}

	~Adjlist() {
		vertex* temp;
		while (src_head != NULL) {
			temp = src_head;
			src_head = src_head->next;
			delete temp;
		}
	}
};


//utility functions-------------------------------------------------------------------------------------------------------------
int timediff(string time_start, string time_end) {				//utility function that converts the given time to seconds and returns
	char c, e;
	int start_hour = 0, start_min = 0, end_hour = 0, end_min = 0;
	stringstream(time_start) >> start_hour >> c >> start_min;		//input start time from string 

	stringstream(time_end) >> end_hour >> e >> end_min;				//take input into endtime from string

	int diff_hour = 0, diff_min = 0;								//to store diff in min and hours separately at first
	diff_hour = end_hour - start_hour;
	diff_min = end_min - start_min;
	if (diff_min > 60)												//convert hours to min
	{
		diff_hour += 1;
		diff_min -= 60;
	}
	else if (diff_min < 0)
	{
		diff_hour -= 1;
		diff_min += 60;
	}
	int total = (diff_hour * 60) + diff_min;
	return total;
}

void UpdateStartTimeDate(string city, Queue<vertex> q, string& t, int& d) {
	Node<vertex>* temp = q.head;
	while (temp != NULL) {
		if (temp->data.destination == city) {
			t = temp->data.flight_details.endtime;
			d = temp->data.flight_details.date;
			int strt_time, end_time;
			stringstream(temp->data.flight_details.starttime) >> strt_time;
			stringstream(temp->data.flight_details.endtime) >> end_time;
			if (strt_time > end_time) d += 1; //In case flight starts on one day and ends on another then increment date to next date
		}
		temp = temp->next;
	}
}

ostream& operator<<(ostream& output, vertex v) {
	v.displayvertex();
	return output;
}

bool findCityInQueue(Queue<vertex> q, string tofind) {
	Node<vertex>* temp = q.head;
	while (temp != NULL) {
		if (temp->data.destination == tofind) return true;
		temp = temp->next;
	}
	return false;
}

vertex returnVertexfromQueue(Queue<vertex> q, string tofind) {
	Node<vertex>* temp = q.head;
	vertex toReturn;
	while (temp != NULL) {
		if (temp->data.destination == tofind) return temp->data;
		temp = temp->next;
	}
	return toReturn;
}

void transit_time(int transitTime, string& flightTime, int& date)
{
	int f_hours, f_mins = 0;
	char c;
	stringstream(flightTime) >> f_hours >> c >> f_mins;
	f_hours = transitTime + f_hours;
	string mins;
	if (f_mins > 60)
	{
		f_hours += 1;
		f_mins -= 60;
	}
	if (f_hours > 24)
	{
		f_hours -= 24;
		date += 1;
	}
	mins = to_string(f_mins);
	flightTime = to_string(f_hours);
	flightTime.append(":");
	flightTime.append(mins);
}

int ComputeTimeThruDate(string time_, int date, vertex first_flight) {
	int toReturn = 0;

	if (date == first_flight.flight_details.date) {
		toReturn = timediff(time_, first_flight.flight_details.starttime);
	}
	else {
		toReturn = timediff(time_, "24:00");
		toReturn += timediff("0:00", first_flight.flight_details.starttime);
	}
	return toReturn;
}

int ComputeTimeThruDate(vertex prev_flight, vertex first_flight) {
	int toReturn = 0;
	int date;
	int prev_strttime, prev_endtime;
	stringstream(prev_flight.flight_details.starttime) >> prev_strttime;
	stringstream(prev_flight.flight_details.endtime) >> prev_endtime;
	if (prev_strttime > prev_endtime) date = prev_flight.flight_details.date + 1;
	else date = prev_flight.flight_details.date;

	if (date == first_flight.flight_details.date) {
		toReturn = timediff(prev_flight.flight_details.endtime, first_flight.flight_details.starttime);
	}
	else {
		toReturn = timediff(prev_flight.flight_details.endtime, "24:00");
		toReturn += timediff("0:00", first_flight.flight_details.starttime);
	}
	return toReturn;
}

int TimeDiffWithDate(string strt_time, string end_time) {
	int strt, end;
	stringstream(strt_time) >> strt;
	stringstream(end_time) >> end;
	int flight_duration = 0;
	if (end < strt) {															//in case flight ends on next day
		flight_duration += timediff(strt_time, "24:00");
		flight_duration += timediff("0:00", end_time);
	}
	else {																		//for normal scenarios where flight starts and ends on same day
		flight_duration = timediff(strt_time, end_time);
	}
	return flight_duration;
}
//-----------------------------------------------------------------------------------------------------------------------------



//=============================================================================================================================
//											-->>  USER INTERFACE <<--
//=============================================================================================================================
int Console_Display() {

	//title page
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		cout << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
		cout << "\t\t============================================================================\t\t\t\t\n";
		cout << "\t\t||";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 59);
		cout << "                                                                         ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		cout << "||\t\t\t\t\n";
		cout << "\t\t||";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 59);
		cout << "           ";
		cout << "WELCOME TO NAAZUK SURATEHAAL FLIGHT RESERVATION SYSTEM";
		cout << "        ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		cout << "|| \t\t\t\t\n";
		cout << "\t\t||";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 59);
		cout << "                                                                         ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		cout << "||\t\t\t\t\n";
		cout << "\t\t============================================================================\t\t\t\t\t\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
		cout << endl << endl << endl << "\t\t\t\t\t";
		system("pause");
		system("CLS");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}

	//option page
	int colour[24];
	for (int i = 0; i < 24; i++) {
		colour[i] = 5;
		if (i == 0) colour[i] = 59;
	}
	int choose = 1;
	char arrow_keys;
	while (true) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 93);
		cout << "<<<-------------------------------ALL ABOARD THE LAST THREE BAINCELLS---------------------------------->>>\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
		cout << "Please select your filters:\n";

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[0]);
		cout << " 1. Origin - Destination - Date - Cost Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[1]);
		cout << " 2. Origin - Destination - Date - Time - Cost Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[2]);
		cout << " 3. Origin - Destination - Data - Time - Airline - Cost Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[3]);
		cout << " 4. Origin - Destination - Date - Time Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[4]);
		cout << " 5. Origin - Destination - Date - Time - Time Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[5]);
		cout << " 6. Origin - Destination - Date - Time - Airline - Time Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[6]);
		cout << " 7. Origin - Destination - Date - Transit Location - Cost Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[7]);
		cout << " 8. Origin - Destination - Date - Time - Transit Location - Cost Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[8]);
		cout << " 9. Origin - Destination - Date - Airline - Transit Location - Cost Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[9]);
		cout << "10. Origin - Destination - Date - Time - Airline - Transit Location - Cost Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[10]);
		cout << "11. Origin - Destination - Date - Transit Location - Min Transit Duration - Cost Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[11]);
		cout << "12. Origin - Destination - Date - Airline - Transit Location - Min Transit Duration - Cost Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[12]);
		cout << "13. Origin - Destination - Date - Time - Transit Location - Min Transit Duration - Cost Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[13]);
		cout << "14. Origin - Destination - Date - Transit Location - Time Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[14]);
		cout << "15. Origin - Destination - Date - Time - Transit Location - Time Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[15]);
		cout << "16. Origin - Destination - Date - Airline - Transit Location - Time Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[16]);
		cout << "17. Origin - Destination - Date - Time - Ailrine - Transit Location - Time Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[17]);
		cout << "18. Origin - Destination - Date - Transit Location - Min Transit Duration - Time Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[18]);
		cout << "19. Origin - Destination - Date - Time - Transit Location - Min Transit Duration - Time Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[19]);
		cout << "20. Origin - Destination - Date - Airline - Transit Location - Min Transit Duration - Time Optimization\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[20]);
		cout << "21. Origin - Destination - Date - Direct Flight\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[21]);
		cout << "22. Origin - Destination - Direct Flight\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[22]);
		cout << "23. Origin Only \n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colour[23]);
		cout << "24. Show All Flights\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 93);
		cout << "\n<<<--------------------------------------------------------------------------------------------------->>>\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		arrow_keys = _getch();
		if (arrow_keys == 80) {
			choose++;
			if (choose > 24) choose = 1;
			for (int i = 0; i < 24; i++) {
				colour[i] = 5;
				if ((i + 1) == choose) colour[i] = 59;
			}
		}
		else if (arrow_keys == 72) {
			choose--;
			if (choose < 1) choose = 24;
			for (int i = 0; i < 24; i++) {
				colour[i] = 5;
				if ((i + 1) == choose) colour[i] = 59;
			}
		}
		else if (arrow_keys == 13) {
			break;
		}
		system("CLS");
	}
	return choose;
}







