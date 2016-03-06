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
			//implement
		}
		~BoundedBuffer(){
			//implement
		}

	/*-----Deposit/Remove----*/
		deposit(string item){
			//implement
		}

		remove(){
			//implement
		}

	};
	


