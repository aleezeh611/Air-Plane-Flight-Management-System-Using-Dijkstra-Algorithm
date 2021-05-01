/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

				Group Members:
				Aleezeh Usman 18i-0529
				Laiba Imran   18i-0477
				Areesha Tahir 18i-1655

				DS FINAL PROJECT : AIRLINE RESERVATION SYSTEM
				Submission Date: 5-11-19

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#pragma once
#include"utility.h"
#include<fstream>
#include<string>

class Graph {
	Adjlist* adjlist;										//dynamiv array storing all sources/vertices
	int cap;												//capacity, will store max number of vertices of sources
	int* charges;
	
	void insertdirectflight(string c, string date, string st, string en, int pr, string airl, int ind) {
		if (ind < cap && adjlist[ind].src_head != NULL && cap != 0)			//just to avoid any errors
			adjlist[ind].insertflight(c, date, st, en, pr, airl);
	}
	void storeNewCity(string city, int& ind) {
		if (adjlist != NULL && cap != 0) {
			for (int i = 0; i < cap; i++) {
				if (adjlist[i].source == "") {
					adjlist[i].InitializeVertex(city);
					ind = i;
					return;
				}
			}
		}
	}
	vertex returnCurrentMin(int weight[], Queue<int> ind, Queue<vertex> vx) {
		vertex toReturn;
		int min = 999999999;
		int index = -1;
		Node<int>* temp = ind.head;
		while (temp != NULL) {
			if (min > weight[temp->data]) {
				index = temp->data;
				min = weight[temp->data];
			}
			temp = temp->next;
		}
		if (index != -1) {
			Node<vertex>* temp2 = vx.head;
			while (temp2 != NULL) {																	//to locate exact vertex that is minimum and return it
				if (adjlist[index].source == temp2->data.destination) toReturn = temp2->data;
				temp2 = temp2->next;
			}
		}
		return toReturn;
	}
	void read_hotelCharges(string filename)
	{
		fstream obj;
		obj.open(filename, ios::in || ios::out);
		string temp;
		charges = new int[cap];
		for (int i = 0; i < cap; i++)
		{
			charges[i] = 0;
		}

		if (obj.is_open())
		{
				while (!obj.eof())
				{
					string City;
					string charge;
					int int_charge = 0;
					obj >> City;
					//cout << City << " ";
					int curr_index = returnIndexesOfAdjList(City);
					//cout << curr_index << " ";
					getline(obj, charge, '\n');
					stringstream(charge) >> int_charge;
					//cout << int_charge << endl;
					charges[curr_index] = int_charge;
				}
			obj.close();
		}
		else
		{
			cout << "--- ERROR::FILE NOT OPEN ---\n";
		}
	}

public:
	Graph() {
		adjlist = NULL;
		charges = NULL;
		cap = 0;
	}

	//Graph creation functions
	void InitializeSystem(string filename, string hotelfile) {						//find max cities airport flies to through file and also charges of hotels in diff cities 
		fstream file;																//since cities dont change very often
		file.open(filename, ios::in || ios::out);
		string tempstr;																//string to store all cities
		int count = 0;																//store number of unique cities

		if (file.is_open()) {
			while (!file.eof()){
				string city;
				file >> city;
				if(tempstr.find(city) == string::npos) {	//check if city already found before 
					tempstr.append(city);					//so only unique cities are counted and no cities counted more than once
					tempstr.append("\n");					//add divider between cities
					count++;
				}
				file >> city;
				if (tempstr.find(city) == string::npos) {	//check if city already found before 
					tempstr.append(city);					//so only unique cities are counted and no cities counted more than once
					tempstr.append("\n");					//add divider between cities
					count++;
				}
				getline(file, city,'\n');
			}
			file.close();
			adjlist = new Adjlist[count];						//create a dynamic array storing all SOURCES
			cap = count;

			file.open(filename, ios::in || ios::out);		//open file again this time to make graph
			if (file.is_open()) {
				string city, date;							//to store all details from file
				string start, end;
				string airline;
				int price;
				int ind;

				while (!file.eof()) {
					file >> city;
					ind = returnIndexesOfAdjList(city);
					if (ind == -1)storeNewCity(city, ind);
					file >>  city;
					int check_dest;
					check_dest = returnIndexesOfAdjList(city);
					if (check_dest == -1)storeNewCity(city, check_dest);
					file >> date >> start >> end >> price>>airline;
					insertdirectflight(city, date,start, end, price,airline, ind);

				}
			}
			else cout<< "--- ERROR::FILE NOT OPEN ---\n";
		}
		else cout << "--- ERROR::FILE NOT OPEN ---\n";

		read_hotelCharges(hotelfile);
	}
	
	//FINDING SHORTEST PATH WITH REFERENCE TO COST/TIME
	bool shortestpathcost(string src, string dest, int& date, string& time, string air_line, int& cheapestcost, vertex& first_flight, int& time_duration) {
		int pathnum = 1;
		bool pathfound = true;																		//check if path at given date is found or not
		bool AllPaths = true;
		int* weight = new int[cap];																	//to store attribute which we will base shortest path on
		int* duration = new int[cap];																//store time of each path
		int src_ind = returnIndexesOfAdjList(src);
		int index = src_ind;
		int dest_index = returnIndexesOfAdjList(dest);
		string* parent = new string[cap];															//store parent of each city in corresponding indexes
		
		for (int i = 0; i < cap; i++) {
			if (i == src_ind) {
				weight[i] = 0;
				duration[i] = 0;
			}
			else{
				weight[i] = 999999999;
				duration[i] = 999999999;
			}
		}
		
		Queue<vertex> Path_S;																		//will store flights required to get to final destination
		Queue<int> All_V;																			//store indexes of sources
		
		for (int i = 0; i < cap; i++) {
			if(i!=src_ind)
			All_V.enqueue(i);
		}

		Queue<vertex> storeVertexesLeft;
		stack<vertex> checkpaths;
		stack<vertex> bestpath;
		int cost = 0;																				//to store the cost of each path
		int org_date = date;																		//store original date and time
		string org_time = time;																	
		int store_cheapest_flight_date = date;														//to store the time and date of cheapest flight 
		string store_cheapest_flight_time = time;													//so next flights from transit can be chosen accordingly
		int store_curr_cheapest = -1;																//store current cheapest - this is needed due to reusing of this function in transit
		int store_time_duration = 0;

		while (AllPaths) {
			while (All_V.find(dest_index)) {															//as long as queue storing all unvisited neighbours has destination we have not reached it thus run loop till there
				vertex u;
				int index_u;
				Queue<vertex> u_neighbours;
				if (index == src_ind) {
					u = *adjlist[index].src_head;														//store starting city in u
					index_u = index;																	//store index of current city we are applying dijkstras on																	
					index++;																			//increment index so that we dont enter this if statement again
				}
				else if (index != src_ind) {
					u = returnCurrentMin(weight, All_V, storeVertexesLeft);								//make neighbour that is not in visited set and has minimum weight the current city
					if (u.destination == "") {															//if no minimum exist and no further path present break loop 
						pathfound = false;
						AllPaths = false;
						break;
					}
					UpdateStartTimeDate(u.destination, storeVertexesLeft, time, date);
					index_u = returnIndexesOfAdjList(u.destination);									//returned city will be removed from queue
					All_V.deleteLocationfromQueue(index_u);
					Path_S.enqueue(u);																	//insert said location into path Queue
					if (u.destination == dest) break;
				}
				u_neighbours = adjlist[index_u].neighbours(date, time, air_line, checkpaths);			//store all neighbours of u which have flights available on given date and time
				if (!findCityInQueue(u_neighbours, dest) && time >= "18:00") {						    //look for flight available on the next day
					time = "0:00";																        //change time to 0 
					date += 1;																	        //change date to next day's date
					Queue<vertex> nextdayneighbours;
					nextdayneighbours = adjlist[index_u].neighbours(date, time, air_line,checkpaths);	//now look for flights on the next day
					if (findCityInQueue(nextdayneighbours, dest)) u_neighbours = nextdayneighbours;
				}
				while (!u_neighbours.E()) {
					vertex curr_neighbor = u_neighbours.dequeue();
					int n_index = returnIndexesOfAdjList(curr_neighbor.destination);
					if (All_V.find(n_index)) {															//to check if it hasnt already been visited before
						int hotelcharges = 0;
						int checkhotelneed = 0;
						if (u.destination != "") {														//to check if any hotel charges are being applied between flights		
							vertex prevflight = returnVertexfromQueue(storeVertexesLeft, u.destination);
							checkhotelneed = ComputeTimeThruDate(prevflight, curr_neighbor);
							if (checkhotelneed > 12*60) {
								hotelcharges = charges[index_u];
							}
						}
						if (weight[n_index] > (weight[index_u] + curr_neighbor.flight_details.ticket_price + hotelcharges)) {
							weight[n_index] = (weight[index_u] + curr_neighbor.flight_details.ticket_price + hotelcharges);
							duration[n_index] = (duration[index_u] + checkhotelneed + TimeDiffWithDate(curr_neighbor.flight_details.starttime, curr_neighbor.flight_details.endtime));
							if (u.destination == "") parent[n_index] = adjlist[index_u].source;			//we start from source itself where destination is NULL
							else parent[n_index] = u.destination;
							storeVertexesLeft.enqueue(curr_neighbor);
						}
					}
				}
			}

			if (pathfound) {
				stack<vertex> ActualShortestPath;												//Path has all shortest distances until our destination found ,
				Node<vertex>* temp_path = Path_S.head;											//now we will filter it out to get our desired route 
				int curr_ind = returnIndexesOfAdjList(dest);
				while (curr_ind != src_ind && curr_ind != -1) {
					Node<vertex>* temp_path = Path_S.head;
					while (temp_path != NULL) {
						if (temp_path->data.destination == adjlist[curr_ind].source) {
							ActualShortestPath.push(*temp_path);								//push path going back from destination onto stack using parents array
							checkpaths.push(*temp_path);
							if (pathnum == 1) bestpath.push(*temp_path);
							break;
						}
						temp_path = temp_path->next;
					}
					curr_ind = returnIndexesOfAdjList(parent[curr_ind]);
				}
				cost = weight[dest_index];																	//store the weight of destination which is basically the total cost

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
				cout << "Path " << pathnum << " From " << src << " to " << dest << ":\n";
				ActualShortestPath.display();
				cout << "Total Duration: " << duration[dest_index] << endl;
				cout << "Total Cost: " << weight[dest_index] << endl << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

			}
			//RESET ALL VARIABLES USED IN DIJKSTRA
			{
				if (pathnum == 1 && weight[dest_index] != 999999999) {
					cheapestcost += weight[dest_index];
					time_duration += duration[dest_index];
					store_time_duration = duration[dest_index];
					store_curr_cheapest = weight[dest_index];
					store_cheapest_flight_date = date;
					store_cheapest_flight_time = time;
				}
				index = src_ind;
				Path_S.C();
				storeVertexesLeft.C();
				All_V.C();
				for (int i = 0; i < cap; i++) {
					if (i != src_ind)
						All_V.enqueue(i);
				}
				for (int i = 0; i < cap; i++) {
					if (i != src_ind) {
						weight[i] = 999999999;
						parent[i] = "";
					}
					else weight[i] = 0;
				}
				time = org_time;
				date = org_date;
				cost = 0;
				pathfound = true;
				pathnum++;
			}
		}

		if (pathfound && store_curr_cheapest != -1) {												//display minimum out of all found paths
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
			cout << "Most Cost Efficient Path from " << src << " to " << dest << ";\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
			bestpath.display();
			cout << "Total Time Duration: " << store_time_duration << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
			cout << " Total Cost :" << store_curr_cheapest << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			cout << "-----------------------------------------------------------------\n\n";

			Node<vertex> temp = bestpath.pop();
			first_flight = temp.data;
		}
		else {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			cout << " <<--- No Path On Day " << org_date << " From " << src << " To " << dest << "--->>" <<endl<< endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			pathfound = false;
		}
		time = store_cheapest_flight_time;															//in case of transit date for next flight should be after current 
		date = store_cheapest_flight_date;															//most cost efficient flight ends
		delete[] weight;
		delete[] parent;
		delete[] duration;
		return pathfound;
	}
	bool GivenTransit(string src, string trans, string dest, int date, string time, string airline, int trans_time, int cost) {
		vertex first_flight;
		int time_duration = 0;
		bool path1 = shortestpathcost(src, trans, date, time, airline, cost, first_flight, time_duration);
		bool path2 = false;
		string store_time = time;
		int store_date = date;
		if (path1) {
			transit_time(trans_time, time, date);
			path2 = shortestpathcost(trans, dest, date, time, airline, cost, first_flight, time_duration);
		}
		if (!path1 || !path2) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
			cout << "\n ==>>> ::No Path Available With The Given Transit Location:: <<==\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			cout << "===================================================================\n\n";
			return false;
		}
		else {
			int checkhotelneed = ComputeTimeThruDate(store_time, store_date, first_flight);
			if (checkhotelneed >= 12*60) {
				int hotelcost = charges[returnIndexesOfAdjList(trans)];
				cost += hotelcost;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
			cout << "TOTAL DURATION: " << time_duration << endl;
			cout << "TOTAL COST OF ENTIRE TRAVEL: " << cost << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			cout << "==================================================================\n\n";
			return true;
		}
	}

	bool shortestpathtime(string src, string dest, int& date, string& time, string air_line, int& leastduration, vertex& first_flight, int& totalcost) {
		int pathnum = 1;
		bool pathfound = true;																		//check if path at given date is found or not
		bool AllPaths = true;
		int* costarr = new int[cap];																//to store cost(as additional info)
		int* weight = new int[cap];																	//to store attribute which we will base shortest path on			
		int src_ind = returnIndexesOfAdjList(src);
		int index = src_ind;
		int dest_index = returnIndexesOfAdjList(dest);
		string* parent = new string[cap];															//store parent of each city in corresponding indexes

		for (int i = 0; i < cap; i++) {																//initialize with a default value
			if (i == src_ind) {
				costarr[i] = 0;
				weight[i] = 0;
			}
			else {
				costarr[i] = 999999999;
				weight[i]  = 999999999;
			}
		}

		Queue<vertex> Path_S;																		//will store flights required to get to final destination
		Queue<int> All_V;																			//store indexes of sources

		for (int i = 0; i < cap; i++) {
			if (i != src_ind)
				All_V.enqueue(i);
		}

		Queue<vertex> storeVertexesLeft;
		stack<vertex> checkpaths;
		stack<vertex> bestpath;
		int flight_duration = 0;																	//to store the time of each path
		int org_date = date;																		//store original date and time
		string org_time = time;
		int store_cheapest_flight_date = date;														//to store the time and date of cheapest flight 
		string store_cheapest_flight_time = time;													//so next flights from transit can be chosen accordingly
		int store_curr_duration = -1;																//store current duration - this is needed due to reusing of this function in transit
		int store_curr_cost = 0;																	//store cost of chosen path

		while (AllPaths) {
			while (All_V.find(dest_index)) {															//as long as queue storing all unvisited neighbours has destination we have not reached it thus run loop till there
				vertex u;
				int index_u;
				Queue<vertex> u_neighbours;
				if (index == src_ind) {
					u = *adjlist[index].src_head;														//store starting city in u
					index_u = index;																	//store index of current city we are applying dijkstras on																	
					index++;																			//increment index so that we dont enter this if statement again
				}
				else if (index != src_ind) {
					u = returnCurrentMin(weight, All_V, storeVertexesLeft);							//make neighbour that is not in visited set and has minimum weight the current city
					if (u.destination == "") {															//if no minimum exist and no further path present break loop 
						pathfound = false;
						AllPaths = false;
						break;
					}
					UpdateStartTimeDate(u.destination, storeVertexesLeft, time, date);
					index_u = returnIndexesOfAdjList(u.destination);									//returned city will be removed from queue
					All_V.deleteLocationfromQueue(index_u);
					Path_S.enqueue(u);																	//insert said location into path Queue
					if (u.destination == dest) break;
				}
				u_neighbours = adjlist[index_u].neighboursT(date, time, air_line, checkpaths);			//store all neighbours of u which have flights available on given date and time
				if (!findCityInQueue(u_neighbours, dest) && time >= "18:00") {						    //look for flight available on the next day
					time = "0:00";																        //change time to 0 
					date += 1;																	        //change date to next day's date
					Queue<vertex> nextdayneighbours;
					nextdayneighbours = adjlist[index_u].neighboursT(date, time, air_line, checkpaths);	//now look for flights on the next day
					if (findCityInQueue(nextdayneighbours, dest)) u_neighbours = nextdayneighbours;
				}
				while (!u_neighbours.E()) {
					vertex curr_neighbor = u_neighbours.dequeue();
					int n_index = returnIndexesOfAdjList(curr_neighbor.destination);
					if (All_V.find(n_index)) {															//to check if it hasnt already been visited before
						int hotelcharges = 0;
						int checkhotelneed = 0;
						if (u.destination != "") {														//to check if any hotel charges are being applied between flights		
							vertex prevflight = returnVertexfromQueue(storeVertexesLeft, u.destination);
							checkhotelneed = ComputeTimeThruDate(prevflight, curr_neighbor);
							if (checkhotelneed > 12 * 60) {
								hotelcharges = charges[index_u];
							}
						}
						if (weight[n_index] > (weight[index_u] + checkhotelneed + TimeDiffWithDate(curr_neighbor.flight_details.starttime, curr_neighbor.flight_details.endtime))) {
							costarr[n_index] = (costarr[index_u] + curr_neighbor.flight_details.ticket_price + hotelcharges);
							weight[n_index] = (weight[index_u] + checkhotelneed + TimeDiffWithDate(curr_neighbor.flight_details.starttime, curr_neighbor.flight_details.endtime));
							if (u.destination == "") parent[n_index] = adjlist[index_u].source;			//we start from source itself where destination is NULL
							else parent[n_index] = u.destination;
							storeVertexesLeft.enqueue(curr_neighbor);
						}
					}
				}
			}

			if (pathfound) {
				stack<vertex> ActualShortestPath;												//Path has all shortest distances until our destination found ,
				Node<vertex>* temp_path = Path_S.head;											//now we will filter it out to get our desired route 
				int curr_ind = returnIndexesOfAdjList(dest);
				while (curr_ind != src_ind && curr_ind != -1) {
					Node<vertex>* temp_path = Path_S.head;
					while (temp_path != NULL) {
						if (temp_path->data.destination == adjlist[curr_ind].source) {
							ActualShortestPath.push(*temp_path);								//push path going back from destination onto stack using parents array
							checkpaths.push(*temp_path);
							if (pathnum == 1) bestpath.push(*temp_path);
							break;
						}
						temp_path = temp_path->next;
					}
					curr_ind = returnIndexesOfAdjList(parent[curr_ind]);
				}
				flight_duration = weight[dest_index];											//store the weight of destination which is basically the total cost

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
				cout << "Path " << pathnum << " From " << src << " to " << dest << ":\n";
				ActualShortestPath.display();
				cout << "Total Cost: " << costarr[dest_index] << endl;
				cout << "Total Duration: " << weight[dest_index] << endl << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

			}
			//RESET ALL VARIABLES USED IN DIJKSTRA
			{
				if (pathnum == 1 && weight[dest_index] != 999999999) {
					leastduration += weight[dest_index];
					totalcost += costarr[dest_index];
					store_curr_duration = weight[dest_index];
					store_curr_cost = costarr[dest_index];
					store_cheapest_flight_date = date;
					store_cheapest_flight_time = time;
				}
				index = src_ind;
				Path_S.C();
				storeVertexesLeft.C();
				All_V.C();
				for (int i = 0; i < cap; i++) {
					if (i != src_ind)
						All_V.enqueue(i);
				}
				for (int i = 0; i < cap; i++) {
					if (i != src_ind) {
						costarr[i] = 999999999;
						parent[i] = "";
						weight[i] =  999999999;
					}
					else {
						costarr[i] = 0;
						weight[i] = 0;
					}
				}
				time = org_time;
				date = org_date;
				flight_duration = 0;
				pathfound = true;
				pathnum++;
			}
		}

		if (pathfound && store_curr_duration != -1) {												//display minimum out of all found paths
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
			cout << "Most Time Efficient Path from " << src << " to " << dest << ";\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
			bestpath.display();
			cout << "Total Cost :" << store_curr_cost << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
			cout << " Total Duration :" << store_curr_duration << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			cout << "-----------------------------------------------------------------\n\n";

			Node<vertex> temp = bestpath.pop();
			first_flight = temp.data;
		}
		else {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			cout << " <<--- No Path On Day " << org_date << " From " << src << " To " << dest << "--->>" << endl << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			pathfound = false;
		}
		time = store_cheapest_flight_time;															//in case of transit date for next flight should be after current 
		date = store_cheapest_flight_date;															//most cost efficient flight ends
		delete[] costarr;
		delete[] parent;
		delete[] weight;
		return pathfound;
	}
	bool GivenTransit_time(string src, string trans, string dest, int date, string time, string airline, int trans_time, int least_duration, int total_cost){
		vertex first_flight;
		bool path1 = shortestpathtime(src, trans, date, time, airline, least_duration, first_flight, total_cost);
		bool path2 = false;
		string store_time = time;
		int store_date = date;
		

		if (path1) {
			transit_time(trans_time, time, date);
			path2 = shortestpathtime(trans, dest, date, time, airline, least_duration, first_flight, total_cost);
		}
		if (!path1 || !path2) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
			cout << "\n ==>>> ::No Path Available With The Given Transit Location:: <<==\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			cout << "===================================================================\n\n";
			return false;
		}
		else {
			int checkhotelneed = ComputeTimeThruDate(store_time, store_date, first_flight);
			least_duration += checkhotelneed;
			if (checkhotelneed >= 12*60) {
				int hotelcost = charges[returnIndexesOfAdjList(trans)];
				total_cost += hotelcost;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
			cout << "TOTAL COST OF ENTIRE TRAVEL: " << total_cost << endl;
			cout << "TOTAL DURATION OF ENTIRE TRAVEL: " << least_duration << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			cout << "==================================================================\n\n";
			return true;
			return true;
		}
	}

	//Direct Flights
	void directFlights(string src, string dest, int date = 0)
	{
		Queue<vertex> direct_flights;
		vertex v;
		int i = returnIndexesOfAdjList(src);								//directly go to the source index

		vertex* temp = adjlist[i].src_head;									//create a temporary pointer to head of list of neighbours of our current source						
		while (temp != NULL)
		{
			if (temp->destination == dest)									//check if we have found our destination
			{
				if (date != 0) {											//if preferred date given check it then enqueue
					if (temp->flight_details.date == date)
					{
						v = *temp;
						if (!direct_flights.find(v)) {						//prevent display of repeated flights
							direct_flights.enqueue(v);
						}
					}
				}
				else if (date == 0)											//if preferred date not given enqueue all available dates 
				{
					v = *temp;
					if (!direct_flights.find(v)) {							//prevent display of repeated flights 
						direct_flights.enqueue(v);
					}
				}
			}
			temp = temp->next;
		}

		cout << endl;
		//display to user
		if (direct_flights.E()) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
			cout << "Not direct flights are available from " << src << " to " << dest;
			if (date != 0) cout << " on given date.";
			cout << endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		else {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
			cout << "All direct flights going from " << src << " to " << dest << " are:\n";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
			direct_flights.display();
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			cout << "=====================================================================\n";
		}
	}

	//helper functions
	void ShowAllFlightsFromCity(string origin) {
		int index = returnIndexesOfAdjList(origin);
		vertex* temp = adjlist[index].src_head->next;
		Queue<vertex> storeflights;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
		cout << "All Flights From " << origin << ":\n";
		while (temp != NULL) {
			if (!storeflights.find(*temp))storeflights.enqueue(*temp);
			temp = temp->next;
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
		storeflights.display();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		cout << "=======================================================\n\n";
	}
	void ShowGraphStructure() {
		for (int i = 0; i < cap; i++) {
			adjlist[i].display();
			cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
		}
	}
	int returnIndexesOfAdjList(string location) {
		if (adjlist != NULL && cap != 0) {
			for (int i = 0; i < cap; i++) {
				if (adjlist[i].source == location) return i;
			}
			return -1;
		}
		else {
			cout << "--- ERROR:: Adjacency List not initialized yet ---\n";
			return -1;
		}
	}

	//destructor
	~Graph() {
		delete[] adjlist;
	}
};


//=========================================MAIN FUNCTION WHERE EVERYTHING IS BEING IMPLEMENTED AND SYSTEM IS BEING RUN=============================================================
void RunSystem() {
	int user_choice = Console_Display();
	Graph gr;
	gr.InitializeSystem("Flights.txt", "HotelCharges_perday.txt");
	//cout << user_choice << endl;
	system("CLS");										//clear screen for BEAUTYYYYYYY :D 

	//basic variables to deal with queries and filters
	string origin = "";									//starting city
	string destination = "";							//destination to reach
	int date = 0;										//preferred date to begin travel
	string strt_time = "0:00";							//time of preferred travel
	string airline = "";								//airline of choice
	string transit_city = "";							//intermediate city to stop in
	int transit_time = 0;								//duration of stay in intermediate city
	int cost = 0;										//cost of travel
	int least_duration = 0;

	cout << "\nPlease enter the following:\n";

	//take inputs based on user choices
	{
		if (user_choice !=24) {
			cout << "Origin City:";
			cin >> origin;
		}
		if (user_choice != 23 && user_choice != 24) {
			cout << "Destination:";
			cin >> destination;
		}
		if (user_choice >= 7 && user_choice <= 20) {
			cout << "Transit Location:";
			cin >> transit_city;
			if ((user_choice >=11 && user_choice <= 13) || (user_choice >= 18 && user_choice <=20)) {
				cout << "Transit duration in hours(rounded off to nearest whole number):";
				cin >> transit_time;
			}
		}
		if (user_choice <  22) {
			cout << "Date of Travel (i.e day of the month available flights are in):";
			cin >> date;
		}
		if (user_choice == 2 || user_choice == 3 || user_choice == 5 || user_choice == 6 || user_choice == 7 ||
			user_choice == 9 || user_choice == 10 || user_choice == 13 || user_choice == 15 || user_choice == 17 ||
			user_choice == 19) {
			cout << "Time in HH:MM format:";
			cin >> strt_time;
		}
		if (user_choice == 3 || user_choice == 6 || user_choice == 9 || user_choice == 10 || user_choice == 12 || 
			user_choice == 16 || user_choice == 17 || user_choice == 20 ) {
			cout << "Preferred Airline:";
			cin >> airline;
		}
	}
	
	cout << endl;
	int checkorigin, checkdest, checktrans;
	checkorigin = gr.returnIndexesOfAdjList(origin);
	checkdest = gr.returnIndexesOfAdjList(destination);
	checktrans = gr.returnIndexesOfAdjList(transit_city);
	if ((origin != "" && checkorigin == -1 ) || (destination != "" && checkdest == -1) || (transit_city != "") && checktrans == -1) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		cout << " <<--- ERROR :: INVALID INPUT --->>\n ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		user_choice = -1;
	}

	//what to do 
	if (user_choice == 1 || user_choice == 2 || user_choice == 3) {
		string default_time = "0:00";
		int storedate = date;
		vertex ignore;
		if (!gr.shortestpathcost(origin, destination, date, strt_time, airline, cost, ignore, least_duration)) {
			cout << "\nAvailability of route a day before:\n";
			date = storedate - 1;
			gr.shortestpathcost(origin, destination, date, default_time, airline, cost, ignore, least_duration);
			cout << "\nAvailability of route a day after:\n";
			date = storedate + 1;
			gr.shortestpathcost(origin, destination, date, default_time, airline, cost, ignore, least_duration);
		}
	}
	else if (user_choice >= 7 && user_choice <= 13) {
		if (!gr.GivenTransit(origin, transit_city, destination, date, strt_time, airline, transit_time, cost)) {
			string default_time = "0:00";
			cout << "\nAvailability of route a day before:\n";
			int checkbeforeafter = date - 1;
			gr.GivenTransit(origin, transit_city, destination, checkbeforeafter, default_time, airline, transit_time, cost);

			cout << "\nAvailability of route a day after:\n";
			checkbeforeafter = date + 1;
			gr.GivenTransit(origin, transit_city, destination, checkbeforeafter, default_time, airline, transit_time, cost);

		}
	}
	else if (user_choice == 21 || user_choice == 22) {
		gr.directFlights(origin, destination, date);
	}
	else if (user_choice == 4 || user_choice == 5 || user_choice == 6) {
		vertex ignore;
		string default_time = "0:00";
		int store_date = date;
		int t_cost = 0;
		if (!gr.shortestpathtime(origin, destination, date, default_time, airline, least_duration, ignore, t_cost)) {
			cout << "\nAvailability of route a day before:\n";
			date = store_date - 1;
			gr.shortestpathtime(origin, destination, date, default_time, airline, least_duration, ignore, t_cost);
			cout << "\nAvailability of route a day after:\n";
			date = store_date + 1;
			gr.shortestpathtime(origin, destination, date, default_time, airline, least_duration, ignore, t_cost);

		}

	}
	else if (user_choice >= 14 && user_choice <= 20) {
		if (!gr.GivenTransit_time(origin, transit_city, destination, date, strt_time, airline, transit_time,least_duration, cost)) {
			string default_time = "0:00";
			cout << "\nAvailability of route a day before:\n";
			int checkbeforeafter = date - 1;
			gr.GivenTransit_time(origin, transit_city, destination, checkbeforeafter, default_time, airline, transit_time, least_duration,cost);

			cout << "\nAvailability of route a day after:\n";
			checkbeforeafter = date + 1;
			gr.GivenTransit_time(origin, transit_city, destination, checkbeforeafter, default_time, airline, transit_time,least_duration, cost);

		}
	}
	else if (user_choice == 23) { 
		gr.ShowAllFlightsFromCity(origin); 
	}
	else if (user_choice == 24) { 
		gr.ShowGraphStructure(); 
	}
}


