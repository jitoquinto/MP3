/* 
    File: semaphore.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/11

*/

#ifndef _semaphore_H_                   // include file only once
#define _semaphore_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   S e m a p h o r e  */
/*--------------------------------------------------------------------------*/

class Semaphore {
private:
  /* -- INTERNAL DATA STRUCTURES
     You may need to change them to fit your implementation. */

  int             value;
  pthread_mutex_t m;
  pthread_cond_t  c;

public:

  /* -- CONSTRUCTOR/DESTRUCTOR */
  //Vanilla implementation may need to be changed
  Semaphore(int _val){
    value = _val;
    pthread_cond_init(&c,NULL);
    pthread_mutex_init(&m,NULL);
  }
  //Vanilla implementation may need to be changed
  ~Semaphore(){
    pthread_cond_destroy(&c);
    pthread_mutex_destroy(&m);
  }

  /* -- SEMAPHORE OPERATIONS */
  //Vanilla implementation of P. Will probably need fixes
  int P(){
    int errNum;//if pthread_lock or unlock dont work
      //lock mutex to restrict access to value before decrement
    if(errNum = pthread_mutex_lock(&m) != 0)
      return errNum;
      //decrement value
    value--;

    //if value is negative then block thread until cond_signal issued
    if(value < 0){
      if(errNum = pthread_cond_wait(&c,&m) != 0)
        return errNum;
    }
    //unlock mutex and allow access to value
    if(errNum = pthread_mutex_unlock(&m) != 0)
      return errNum;
    //P() executed correctly 
    return  0;
  }

  //Vanilla implementation of V(). Might need fixes
  int V(){
    int errNum;//used to check errors that mutex or cond might have

    pthread_mutex_lock(&m);//lock mutex to increment value in critical section

    value++;
    if(value <= 0){
      if(errNum = pthread_cond_signal(&c) != 0)
        return errNum;
    }
    pthread_mutex_unlock(&m);
  }
};


#endif


