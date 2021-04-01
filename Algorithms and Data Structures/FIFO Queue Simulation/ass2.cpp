/*-----Assignment 2-----*/
/*
	Student name: Thi Thuy Trang Nguyen
	Login: tttn941
	Student ID: 6166994
*/

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

enum EventType {CustomerArrival, ServerFinish};

class Customer
{
	private:
		double arrival;
		double tally;
		bool card; //true if pay by card
	public:
		static int numberOfCust;
		Customer(double a, double t, string method); //overloaded constructor
		Customer(){};
		Customer(double a, double t, bool method);
		double getArrivalTime();
		double getTallyTime();
		bool usesCard();
		double getPaymentTime();
};
int Customer::numberOfCust = 0;

class CustomerQueue //FIFO queue
{
	private:
		int MAX_LENGTH;
		int length;
		double sumOfLengths;
		double averageLength;
		double totalTimeOfAllCust;
		int start;
		int next;
		Customer queue[500];
	public:
		CustomerQueue();
		void enqueue(Customer,double,double);
		Customer* dequeue(double,double);
		bool isEmpty();
		int getMaxLength();
		double getAveLength(double);
		double getTotalTimeOfAllCust();
};

class Server
{
	private:
		int id;
		double efficiency;
		bool busy;
		int numberOfCustServed;
		double timeSpentBusy;
	public:
		Server(int id, double e);
		Server(){busy = false;};
		double getEfficiency();
		void changeBusyStatus(bool stat);
		bool isBusy();
		int getID();
		void incrementNumberOfCustServed();
		int getNumberOfCustServed();
		void calcBusyTime(double);
		double getIdleTime(double);
};

class ServerArray
{
	private:
		Server array[20];    //normal array of servers
		Server idleServers[20]; //min heap of idle servers according to their efficiency
		int numberOfServers;
		int numberOfIdleServers;
		
	public:
		ServerArray(int no);
		void addServer(Server);
		void siftdown(int i);
		void siftup(int i);
		void makeIdle(Server, double);
		void makeBusy(double);
		bool isAllBusy();
		Server findFastestServer();
		void printIdleOnes();
		void displayStatsForEachServer(double);
};
void swap(Server &s1, Server &s2);


class Event
{
	private:
		EventType type;
		double time;
		double tally;
		bool card;
		Server servedBy;
		
	public:
		Event(){}; //constructor
		Event(EventType eType, double eTime, double tallyTime, bool c);
		Event(EventType eType, double eTime, double tallyTime, bool c, Server &s);
		EventType getEventType();
		double getEventTime();
		double getTallyTime();
		double getPaymentTime();
		bool getPaymentMethod();
		Server& getServer();
};

class EventQueue  //min heap of events according to their event time
{
	private:
		Event queue[100];
		int numberOfEvents;
	public:
		EventQueue();
		void siftdown(int i);
		void siftup(int i);
		void enqueue(Event e);
		Event dequeue();
		bool isEmpty();
		int totalevent;
};

void swap(Event &event1, Event &event2);

int main() //driver
{
	/*
	//Step 1
	//test customer queue and event queue
	CustomerQueue cqueue;
	EventQueue equeue;
	cout << "=====Enqueue customers=====" << endl;
	for(int i = 1; i <= 10; i++)
	{
		Customer cust(i, rand() % 100 + 1, 0, false);
		cout << "Arrival time of customer " << i << " is: " << cust.getArrivalTime() << endl;
		cqueue.enqueue(cust);
	}
	cout << "=====Dequeue customers and enqueue events=====" << endl;
	while (!cqueue.isEmpty())
	{
		Customer c = cqueue.dequeue();
		cout << "Arrival time of dequeued customer: " << c.getArrivalTime() << endl;
		Event event(CustomerArrival, c.getArrivalTime(), 0, false);
		equeue.enqueue(event);
	}
	cout << "=====Dequeue events=====" << endl;
	while(!equeue.isEmpty())
	{
		Event e = equeue.dequeue();
		cout << "Event time of dequeued event: " << e.getEventTime() << endl;
	}
	*/
	
	//Step 2 - ServerArray implementation
	int numberOfServers = 0;
	ifstream inData;
	inData.open("ass2.txt");
	if(!inData) //check if file opened successfully
	{
		cerr << "Error opening: ass2.txt" << endl;
		return 1;
	}
	
	//add servers to server array, they are all initially idle
	inData >> numberOfServers;
	ServerArray serversArr(numberOfServers);
	for (int i = 0; i < numberOfServers; i++)
	{
		double efficiency;
		inData >> efficiency;
		Server s(i,efficiency);
		serversArr.addServer(s);
	}
	
	//step 3 - data processing
	double firstArrival, tally; //the arrival time and tally time of first customer
	string paymentMethod;
	inData >> firstArrival;
	inData >> tally;
	inData >> paymentMethod;
	Customer::numberOfCust++;
	Customer c(firstArrival, tally, paymentMethod);
	EventQueue equeue;
	CustomerQueue cqueue;
	Event event(CustomerArrival, firstArrival, tally, c.usesCard());
	equeue.enqueue(event);
	double timeSinceLastChange = firstArrival;
	double currentTime = firstArrival;
	int totalQueuedCust = 0;
	int totalServedCust = 0;
	
	while(!equeue.isEmpty())
	{
		
		Event e = equeue.dequeue();
		currentTime = e.getEventTime();
		if (e.getEventType() == CustomerArrival)
		{
			if(!serversArr.isAllBusy())
			{
				Server serv = serversArr.findFastestServer();
				double serviceTime = (e.getTallyTime() * serv.getEfficiency()) + e.getPaymentTime();
				serversArr.makeBusy(serviceTime);
				equeue.enqueue(Event(ServerFinish, currentTime+serviceTime, e.getTallyTime(), e.getPaymentMethod(), serv));
				
			}else{
				totalQueuedCust++;
				cqueue.enqueue(Customer(currentTime, e.getTallyTime(), e.getPaymentMethod()), timeSinceLastChange, currentTime);
				timeSinceLastChange = currentTime;
			}
			
			double arrivalTime;
			double tallyTime;
			string payment;
			string custDetail;
			if(inData >> arrivalTime >> tallyTime >> payment) //if it's not end of file, read next line
			{
				Customer::numberOfCust++;
				Customer cust(arrivalTime, tallyTime, payment);
				equeue.enqueue(Event(CustomerArrival,arrivalTime, tallyTime, cust.usesCard()));
			}
			
		}else{ //ServerFinish event
			Server s = e.getServer();
			serversArr.makeIdle(s, currentTime);
			totalServedCust++;
			if(!cqueue.isEmpty())
			{
				Customer* cust = cqueue.dequeue(timeSinceLastChange, currentTime);
				timeSinceLastChange = currentTime;
				Server serv = serversArr.findFastestServer();
				double serviceTime = ((*cust).getTallyTime() * serv.getEfficiency()) + (*cust).getPaymentTime();
				serversArr.makeBusy(serviceTime);
				equeue.enqueue(Event(ServerFinish, currentTime+serviceTime, (*cust).getTallyTime(), (*cust).usesCard(), serv));
				
			}
		}
		
	}
	inData.close(); //close input file
	//printing stats
	int totalCustomer = Customer::numberOfCust;
	double serviceEndTime = currentTime; //the last ServerFinish event time
	//cout << std::fixed;
	//cout << std::setprecision(3);
	double totalServingTime = serviceEndTime - firstArrival;
	cout << fixed << setprecision(2);
	cout << "Total number of customers served: " << totalCustomer << endl;
	cout << "Time taken to serve all customers: " << totalServingTime << endl;
	cout << "The greatest length reached by the customer queue: " << cqueue.getMaxLength() << endl;
	cout << "The average length of the customer queue: " << cqueue.getAveLength(serviceEndTime) << endl;
	cout << "The average time spent by a customer in the customer queue: " << cqueue.getTotalTimeOfAllCust()/totalCustomer << endl;
	double percent = (totalCustomer-totalQueuedCust)*100.00f/totalCustomer;
	cout << "The percentage of customers who’s waiting time in the customer queue was 0 (zero): " << percent << "%" << endl;
	serversArr.displayStatsForEachServer(serviceEndTime);
	
	return 0;
}

//implement methods in Customer class
Customer::Customer(double a, double t, string method)
{
	arrival = a;
	tally = t;
	if (method == "card")
	{
		card = true;
	}else{
		card = false;
	}
}

Customer::Customer(double a, double t, bool method)
{
	arrival = a;
	tally = t;
	card = method;
}

double Customer::getArrivalTime()
{
	return arrival;
}
double Customer::getTallyTime()
{
	return tally;
}
double Customer::getPaymentTime()
{
	if (card)
	{
		return 0.7;
	}else{
		return 0.3;
	}
}
bool Customer::usesCard()
{
	return card;
}

//implement methods in Server class
Server::Server(int ID, double e)
{
	id = ID;
	efficiency = e;
	busy = false;
	numberOfCustServed = 0;
	timeSpentBusy = 0;
}
int Server::getID()
{
	return id;
}
double Server::getEfficiency()
{
	return efficiency;
}
bool Server::isBusy()
{
	return busy;
}
void Server::changeBusyStatus(bool stat)
{
	busy = stat;
}
int Server::getNumberOfCustServed()
{
	return numberOfCustServed;
}
void Server::incrementNumberOfCustServed() //for counting the number of customers each server served
{
	numberOfCustServed++;
}
void Server::calcBusyTime(double i) //calculate the busy time of each server
{
	timeSpentBusy += i;
}

double Server::getIdleTime(double totalTime) //calculate the time the server spent idle
{
	double idle = totalTime - timeSpentBusy;
	return idle;
}
//implement methods in CustomerQueue class
CustomerQueue::CustomerQueue()
{
	start = 1;
	next = 1;
	length = 0;
	MAX_LENGTH = 0;
	averageLength = 0;
}
int CustomerQueue::getMaxLength()
{
	return MAX_LENGTH;
}

double CustomerQueue::getAveLength(double totalQueueTime) //average queue length over time
{
	averageLength = sumOfLengths/totalQueueTime;
	return averageLength;
}
void CustomerQueue::enqueue(Customer c, double timeSinceLastChange, double enqueueTime)
{
	sumOfLengths += ((enqueueTime - timeSinceLastChange) * length); //calculate the average queue length
	totalTimeOfAllCust -= enqueueTime; //calculating the total time the customers spent in the queue
	length++;
	if(length > MAX_LENGTH)
		MAX_LENGTH = length;
	
	queue[next] = c;
	next++;
	if (next >= 500)
	{
		next = 1;
	}
}
Customer* CustomerQueue::dequeue(double timeSinceLastChange, double dequeueTime)
{
	sumOfLengths += ((dequeueTime - timeSinceLastChange) * length); //calculating the average queue length
	totalTimeOfAllCust += dequeueTime; //calculating the total time the customers spent in the queue
	if (start == next)
	{
		length = 0;
		//return Customer();
		return NULL;
	}
	length--;
	Customer &c = queue[start];
	start++;
	if(start >= 500)
		start = 1;
	return &c;
}
bool CustomerQueue::isEmpty()
{
	if (start == next)
	{
		return true;
	}
	return false;
}
double CustomerQueue::getTotalTimeOfAllCust()
{
	return totalTimeOfAllCust;
}

//implement methods for Event class
//overloaded constructors
Event::Event(EventType eType, double eTime, double tallyTime, bool c)
{
	type = eType;
	time = eTime;
	tally = tallyTime;
	card = c;
	
}
Event::Event(EventType eType, double eTime, double tallyTime, bool c, Server &s)
{
	type = eType;
	time = eTime;
	tally = tallyTime;
	card = c;
	servedBy = s;
}

Server& Event::getServer()
{
	return servedBy;
}
double Event::getEventTime()
{
	return time;
}
double Event::getTallyTime()
{
	return tally;
}
double Event::getPaymentTime()
{
	if (card)
	{
		return 0.7;
	}else{
		return 0.3;
	}
}
EventType Event::getEventType()
{
	return type;
}
bool Event::getPaymentMethod()
{
	return card;
}

//implement methods for EventQueue class
EventQueue::EventQueue()
{
	numberOfEvents = 0;
}
void EventQueue::siftdown(int i)
{
	int c = i*2;
	if(c <= numberOfEvents)
	{
		if(c+1 <= numberOfEvents)
		{
			if(queue[c].getEventTime() > queue[c+1].getEventTime())
			{
				c++;
			}
		}
		if(queue[i].getEventTime() > queue[c].getEventTime())
		{
			swap(queue[i], queue[c]);
			siftdown(c);
		}
	}
}

void EventQueue::siftup(int i)
{
	int p = i/2;
	if(p >= 1)
	{
		if(queue[p].getEventTime() <= queue[i].getEventTime())
		{
			return;
		}else{
			swap(queue[p], queue[i]);
			siftup(p);
		}
	}
}
void EventQueue::enqueue(Event e)
{
	numberOfEvents++;
	queue[numberOfEvents] = e;
	siftup(numberOfEvents);
}

Event EventQueue::dequeue()
{
	swap(queue[numberOfEvents], queue[1]);
	Event e = queue[numberOfEvents];
	numberOfEvents--;
	siftdown(1);
	return e;
}
bool EventQueue::isEmpty()
{
	if(numberOfEvents <= 0 )
	{
		return true;
	}
	return false;
}

void swap(Event &event1, Event &event2)
{
	Event temp = event1;
	event1 = event2;
	event2 = temp;
}

//implement methods for ServerArray class
ServerArray::ServerArray(int no)
{
	numberOfServers = no;
	numberOfIdleServers = 0;
}
void ServerArray::siftdown(int i)
{
	int c = i*2;
	if(c < numberOfIdleServers)
	{
		if(c+1 <= numberOfServers)
		{
			if(idleServers[c].getEfficiency() > idleServers[c+1].getEfficiency())
			{
				c++;
			}
		}
		if(idleServers[i].getEfficiency() > idleServers[c].getEfficiency()){
			swap(idleServers[i], idleServers[c]);
			siftdown(c);
		}
	}
}

void ServerArray::siftup(int i)
{
	int p = i/2;
	if(p >= 1)
	{
		if(idleServers[p].getEfficiency() <= idleServers[i].getEfficiency())
		{
			return;
		}else{
			swap(idleServers[p], idleServers[i]);
			siftup(p);
		}
	}
}
void ServerArray::makeIdle(Server s, double finishTime)
{
	s.changeBusyStatus(false); //change the busy flag
	numberOfIdleServers++;
	idleServers[numberOfIdleServers] = s; //add the server to the idle array
	siftup(numberOfIdleServers);
}

void ServerArray::makeBusy(double serviceTime)
{
	
	int servID = idleServers[1].getID();
	array[servID].incrementNumberOfCustServed(); //count the number of customers served by the server
	array[servID].calcBusyTime(serviceTime); //calculate the period the server is busy
	//remove the server from the idle server list
	swap(idleServers[numberOfIdleServers], idleServers[1]);
	numberOfIdleServers--;
	siftdown(1);
}
bool ServerArray::isAllBusy()
{
	if(numberOfIdleServers <= 0)
	{
		return true;
	}
	return false;
}
Server ServerArray::findFastestServer() //the fastest one is the first one in the min heap
{
	return idleServers[1];
}
void ServerArray::addServer(Server s) //add server to the normal server array and make the servers initially idle
{
	array[s.getID()] = s;
	makeIdle(s,0);
}

void swap(Server &s1, Server &s2)
{
	Server temp = s1;
	s1 = s2;
	s2 = temp;
}


void ServerArray::displayStatsForEachServer(double totalTime)
{
	cout << endl;
	cout << "\tServer" << "\tEfficiency" << "\tCustomersServed" << "\tIdleTime" << endl;
	for (int i = 0; i < numberOfServers; i++)
	{
		Server s = array[i];
		cout << fixed;
		cout << setprecision(2);
		cout << setw(11) << right;
		cout << i << setw(11)  << s.getEfficiency() << setw(18) << s.getNumberOfCustServed() << setw(14) << s.getIdleTime(totalTime) << endl;
	}
}

/*----Step 5: Specification ----*/
/*
	1. Data structures used in the program:
	-The customer array is a normal FIFO queue as it allows for access to its elements in O(x) as opposed to using a heap.
	-The event queue is a min heap as it is a priority queue ordered by  the event time. A heap makes it faster to achieve the element with the smallest event time.
	-The server array consists of a normal array of servers to do the stats and a min heap ordered by their efficiency to get the most efficient one.
	2. Algorithm used:
	-To count the number of customers served, simply increment the counter when a new customer is read from the file.
	-To calculate the time taken to serve all customers, read the first arrival time and the last ServerFinish event. The total time = last finish event - first arrival.
	-To find the greatest length reached by the customer queue, I used a linear search to find the longest length whenever CustomerQueue.enqueue is called.
	-To find the average length of the customer queue, I calculated the sum of the queue length overtime whenever enqueue and dequeue are called.
	Then I divided the sum by the total time it took to serve all customers.
	-To calculate the average time spent by a customer in the queue, I summed the time taken for those who had to queue. The summation is performed whenever enqueue or dequeue is called.
	Then I divided the sum by the total customers calculated above.
	-To calculate the percentage of customers who don't have to wait in the queue, I count the ones who have to wait in the queue then minus the total number of customers with the result.
	The counter of customers who have to queue is increment whenever enqueue is called.
	-I added an attribute to keep track of the number of customers each server has to served. Since each server has an ID number corresponding to its index in the server array, counting the customers they served is fast when using indecies.
	An attribute is also added to keep track of the time they are busy. Thus, the time they spent idle is calculated as the difference between the total time to serve all customers and the time each server was busy. These are done when makeBusy and makeIdle are called.
	3. Speed:
	-Min heaps are used to enhance speed as there are 2 priority queues in the program: event queue and the heap of idle servers.
	-In terms of the customer queue, since it is not a priority queue, a normal FIFO queue best fits because it only takes O(n) steps to enqueue and dequeue the customers.
	-Stats are done when enqueuing and dequing the customers so it takes O(n) to do the stats.
*/
