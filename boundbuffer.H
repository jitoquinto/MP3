/*
	File: boundbuffer.C
	Author: Jesus Toquinto
	CSCE 313 Section 503
	Implementation of a bounded buffer for a producer consumer
*/

	#include <queue>
	#include <string>
	#include <pthread.h>
	#include "semaphore.H"

	using namespace std;

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
		queue<string> buffer;
	public://public functions

	/*-----Constructor/Destructor------*/
		BoundedBuffer(int size){
			full = new Semaphore(0);
			empty = new Semaphore(size);
			pthread_mutex_init(&m,NULL);
			count = 0;
		}
		~BoundedBuffer(){
			pthread_mutex_destroy(&m);
			delete full;
			delete empty;
		}

	/*-----Deposit/Remove----*/
		void deposit(string item){
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

			//succeeded 
			return 0;

		}

		string remove(){
			//implement
			int errNum;
			string item;
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
			return string;
		}

	};
	

