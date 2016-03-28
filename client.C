/* 
    File: client.C

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <unistd.h>
#include <iomanip>
#include <stdlib.h>

#include "reqchannel.H"
#include "semaphore.H"
#include "boundbuffer.H"

using namespace std;

/*--------------------------------------------------------------------------*/
/* Thread Arguments */ 
/*--------------------------------------------------------------------------*/
typedef struct WORK_THREAD_ARGS{
  //Buffers
  BoundedBuffer* requests;
  BoundedBuffer* joe_stat;
  BoundedBuffer* jane_stat;
  BoundedBuffer* john_stat;
  //Request Channel
  RequestChannel* worker_channel;
}WORK_THREAD_ARGS;


typedef struct stat_map{
  int id;
  //ID's Jane:0; John:1; Joe:2;
  BoundedBuffer* stat_buffer;
}STAT_MAP;


typedef struct REQUEST_THREAD_ARGS{
  BoundedBuffer* buffer;
  int id;
  int total_requests;
}REQUEST_THREAD_ARGS;

 //histograms globals (fix to not be global later)
    vector<int> hist_jane(100);
    vector<int> hist_john(100);
    vector<int> hist_joe(100);
/*--------------------------------------------------------------------------*/
/* Functions */
/*--------------------------------------------------------------------------*/
void showHelp(){
  cout<<"How to use: "<<endl;
  cout<<"client [-n <requests>][-b <buffer size>][-w <worker threads>]"<<endl;
  cout<<"<request number> is the number of requests per person."<<endl;
  cout<<"<buffer size> is the the size of the requests bounded buffer."<<endl;
  cout<<"<worker threads> is the number of worker threads there will be."<<endl;
}

/*--------------------------------------------------------------------------*/
/* Thread Functions */
/*--------------------------------------------------------------------------*/
void* request_thread_function(void* args){
  //deposit requests into buffer
  REQUEST_THREAD_ARGS* args1 = (REQUEST_THREAD_ARGS*)args;
  RequestElement request;
  if(args1->id == 0){
    request.id = 0;
    request.s = "data Jane Smith";
  }
  else if(args1->id == 1){
    request.id = 1;
    request.s = "data John Doe";
  }
  else if(args1->id ==2){
    request.id = 2;
    request.s = "data Joe Smith";
  }
  //deposit n times for each person
  for(int i=0; i<args1->total_requests; i++){
    args1->buffer->deposit(request);
  }
  cout<<"Requests for id number " << request.id << " completed."<<endl;
}


void* worker_thread_function(void* args){
  WORK_THREAD_ARGS* args1 = (WORK_THREAD_ARGS*)args;
  RequestElement send, reply;
  while(true){
    send = args1->requests->remove();
    if(send.s == "KILL"){
      args1->requests->deposit(send);//redeposit kill for other threads to quit
      break;
    }
    //send request to server
    reply.s = args1->worker_channel->send_request(send.s);
    reply.id = send.id;
    //deposit into appropriate stat buffer
    if(reply.id == 0)
      args1->jane_stat->deposit(reply);
    if(reply.id == 1)
      args1->john_stat->deposit(reply);
    if(reply.id == 2)
      args1->joe_stat->deposit(reply);

  }
//if were here kill has been called. lets get outta here
  args1->worker_channel->send_request("quit");
}

void* stat_thread_function(void* args){
  STAT_MAP* person = (STAT_MAP*)args;
  RequestElement temp;
  while(true){//deposit into histogram
    temp = person->stat_buffer->remove();
    if(temp.id == 0)
      hist_jane[atoi(temp.s.c_str())] += 1;
    if(temp.id == 1)
      hist_john[atoi(temp.s.c_str())] +=1;
    if(temp.id == 2)
      hist_joe[atoi(temp.s.c_str())] += 1;
   }
  cout<<"Stat thread ran"<<endl;
}

void print_hist(STAT_MAP* dude){
  vector<int> hist;
  if(dude->id == 0)
    hist = hist_jane;
  if(dude->id == 1)
    hist = hist_john;
  if(dude->id ==2)
    hist = hist_joe;

  vector<int> tens_hist(10);
  for(int i = 0; i<hist.size();i++){
    if(hist[i]<10)
      tens_hist[0]++;
    if(hist[i]<20 && hist[i]>=10)
      tens_hist[1]++;
    if(hist[i]<30 && hist[i]>=20)
      tens_hist[2]++;
    if(hist[i]<40 && hist[i]>=30)
      tens_hist[3]++;
    if(hist[i]<50 && hist[i]>=40)
      tens_hist[4]++;
    if(hist[i]<60 && hist[i]>=50)
      tens_hist[5]++;
    if(hist[i]<70 && hist[i]>=60)
      tens_hist[6]++;
    if(hist[i]<80 && hist[i]>=70)
      tens_hist[7]++;
    if(hist[i]<90 && hist[i]>=80)
      tens_hist[8]++;
    if(hist[i]<100 && hist[i]>=90)
      tens_hist[9]++;
  }
  cout<<setw(8)<<"0-9"<<setw(8)<<"10-19"<<setw(8)<<"20-29"<<setw(8)<<"30-39"<<setw(8)<<"40-49"<<setw(8)<<"50-59"<<setw(8)<<"60-69"<<setw(8)<<"70-79"<<setw(8)<<"80-89"<<setw(8)<<"90-99"<<endl;
  for(int i =0; i<tens_hist.size();i++){
    cout<<setw(8)<<hist[i];
  }
}
/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
//fork off a process and start data server
  pid_t pid = fork();
  if(pid == 0){
    execv("dataserver",argv);
  }
  //Parent thread
  else{
    //get arguments from command line
    /*-------Default values if none entered------*/
    int request_number = 1000;
    int worker_threads_count = 35;
    int buffer_size = 250;

    int c;
    while((c = getopt(argc,argv,"n:b:w:")) != -1)
      switch(c){
        case 'n':
          request_number = atoi(optarg);
          break;
        case 'b':
          buffer_size = atoi(optarg);
          break;
        case 'w':
          worker_threads_count = atoi(optarg);
          break;
        case '?':
          cout<<"Error unknown option"<<endl;
          showHelp();
          abort();
        default:
          cout<<"Error"<<endl;
          showHelp();
          abort();
      }

    //Coordinator: Starts client and all elements
    //Create Bounded Buffers
    BoundedBuffer* request_buffer = new BoundedBuffer(buffer_size);
    BoundedBuffer* janes_buffer = new BoundedBuffer(buffer_size);
    BoundedBuffer* johns_buffer = new BoundedBuffer(buffer_size);
    BoundedBuffer* joes_buffer = new BoundedBuffer(buffer_size);

    //initialize pthreads
    pthread_t request_threads[3];
    pthread_t worker_threads[worker_threads_count];
    pthread_t stat_threads[3];

    //initialize worker arguments
    WORK_THREAD_ARGS* worker_args[worker_threads_count];

    //Initialize stat args
    STAT_MAP* jane;
    jane->id = 0;
    jane->stat_buffer = janes_buffer;
    STAT_MAP* john;
    john->id = 1;
    john->stat_buffer = johns_buffer;
    STAT_MAP* joe;
    joe->id = 2;
    joe->stat_buffer = joes_buffer;


    //initialize request arguments
    REQUEST_THREAD_ARGS* jane_request;
    jane_request->buffer = request_buffer;
    jane_request->id = 0; 
    jane_request->total_requests = request_number;
    REQUEST_THREAD_ARGS* john_request;
    john_request->buffer = request_buffer;
    john_request->id = 1;
    john_request->total_requests = request_number;
    REQUEST_THREAD_ARGS* joe_request;
    joe_request->buffer = request_buffer;
    joe_request->id = 2;
    joe_request->total_requests = request_number;

    //create control channel
    cout << "CLIENT STARTED:" << endl;

    cout << "Establishing control channel... " << flush;
    RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
    cout << "done." << endl;

    //create worker threads
    for(int i = 0; i < worker_threads_count; i++){
      string new_channel_name = chan.send_request("newthread");
      RequestChannel* temp = new RequestChannel(new_channel_name,RequestChannel::CLIENT_SIDE);
      worker_args[i]->requests = request_buffer;
      worker_args[i]->joe_stat = joes_buffer;
      worker_args[i]->jane_stat = janes_buffer;
      worker_args[i]->john_stat = johns_buffer;
      worker_args[i]->worker_channel = temp;
      pthread_create(&worker_threads[i],NULL,worker_thread_function,(void*)worker_args[i]);
    }

    //create stat threads
    pthread_create(&stat_threads[0],NULL,stat_thread_function,(void*)jane);
    pthread_create(&stat_threads[1],NULL,stat_thread_function,(void*)john);
    pthread_create(&stat_threads[2],NULL,stat_thread_function,(void*)joe);

    //create request threads
    pthread_create(&request_threads[0],NULL,request_thread_function,(void*)jane_request);
    pthread_create(&request_threads[1],NULL,request_thread_function,(void*)john_request);
    pthread_create(&request_threads[2],NULL,request_thread_function,(void*)joe_request);

    /*------------ DESTROY EVERYTHING---------------*/
    //well wait for it to finish
    for(int i = 0; i<3; i++){
      pthread_join(request_threads[i],NULL);
    }
    //deposit kill request to stop the worker threads
    //remember to make sure worker threads redeposit this 
    //request so all threads are killed
    RequestElement kill;
    kill.s = "KILL";
    request_buffer->deposit(kill);

    //now lets wait for worker threads to end
    for(int i = 0; i<worker_threads_count;i++){
      pthread_join(worker_threads[i],NULL);
    }

    //kill requests for stat threads will be deposited within worker thread 
    //so lets just wait for that 
    for(int i = 0; i<3; i++){
      pthread_join(stat_threads[i],NULL);
    }
    //print it all
    cout<<"Jane Smith's Histogram"<<endl;
    print_hist(jane);
    cout<<"John Doe's Histogram"<<endl;
    print_hist(john);
    cout<<"Joe Smith's Histogram"<<endl;
    print_hist(joe);
    //Everything should be closed now. Lets end this 
    string close_reply = chan.send_request("quit");
    cout<<close_reply<<endl;
    usleep(1000000);

  }

}
