/*
	File: boundbuffer.C
	Author: Jesus Toquinto
	CSCE 313 Section 503
	Implementation of a bounded buffer for a producer consumer
*/

	#include <queue>
	#include <string>
	#include <pthread.h>
	#include <iostream>
	#include "semaphore.H"

	using namespace std;
	class RequestElement{
	public:
		int id;
		string s;

		RequestElement(){
			id = 0;
			s = " ";
		}
	};

	class BoundedBuffer{
	private://private variables	
		//keeps track of bounded buffer bounds
		Semaphore full;
		Semaphore empty;
		//to protect critical section
		pthread_mutex_t m;
		//number of items in buffer
		int count;
		//buffer string just for firsts implementation. will be changed.
		queue<RequestElement> buffer;
	public://public functions

	/*-----Constructor/Destructor------*/
		BoundedBuffer(int size):full(0),empty(size),count(0)
		{
			pthread_mutex_init(&m,NULL);
		}
		~BoundedBuffer(){
			pthread_mutex_destroy(&m);
		}

	/*-----Deposit/Remove----*/
		void deposit(RequestElement item){
			int errNum;
			//decrease empty and check if P was succesful
			if(errNum = empty.P() != 0)
				cout<<" empthy.P() not succesful with error number: "<<errNum<<endl;
			//Critical Section
			if(errNum = pthread_mutex_lock(&m) != 0)
				cout<<" mutex lock not succesful with error number: "<<errNum<<endl;
			buffer.push(item);//push item to buffer
			//Leave Critical Section
			if(errNum = pthread_mutex_unlock(&m) != 0)
				cout<<" mutex unlock not succesful with error number: "<<errNum<<endl;
			//increase full
			if(errNum = full.V() != 0)
				cout<<" full.V() not succesful with error number: "<<errNum<<endl;

		}

		RequestElement remove(){
			//implement
			int errNum;
			RequestElement item;
			//decrease full and check if P sucessful
			if(errNum = full.P() != 0)
				cout<<"full.P() not successful with error number: "<<errNum<<endl;
			//enter critical section
			if(errNum = pthread_mutex_lock(&m) != 0)
				cout<<"mutex lock not successful with error number: "<<errNum<<endl;
			//pop from vector
			item = buffer.front();
			buffer.pop();
			if(errNum = pthread_mutex_unlock(&m) != 0)
				cout<<"mutex unlock unsuccesful with error number: "<<errNum<<endl;
			if(errNum = empty.V() != 0)
				cout<<"empty.V() not successful with error number: "<<errNum<<endl;
			return item;
		}

	};
	


