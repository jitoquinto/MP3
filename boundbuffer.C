/*
	File: boundbuffer.C
	Author: Jesus Toquinto
	CSCE 313 Section 503
	Implementation of a bounded buffer for a producer consumer
*/

	#include <vector>
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
		vector<string> buffer;
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
		int deposit(string item){
			int errNum;
			//decrease empty and check if P was succesful
			if(errNum = empty.P() != 0)
				return errNum;
			//Critical Section
			if(errNum = pthread_mutex_lock(&m) != 0)
				return errNum;
			buffer.push_back(item);//push item to buffer
			//Leave Critical Section
			if(errNum = pthread_mutex_unlock(&m) != 0)
				return errNum;
			//increase full
			if(errNum = full.V() != 0)
				return errNum;

			//succeeded 
			return 0;

		}

		remove(){
			//implement
		}

	};
	


