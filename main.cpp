#include <iostream>
#include <vector>
#include <thread>
#include "barrier.hpp"
#include "xorshift.hpp"

#ifdef L2
#include "list2.hpp"
#else
#include "list1.hpp"
#endif

using namespace std;
using namespace barrier;

const int threadsnum = 4;

// success, failure
std::pair<int,int> insertcounter[threadsnum];
std::pair<int,int> removecounter[threadsnum];

void run(int id, Barrier *sb, Barrier *eb, test::TSXlist *ls){
  const int keyrange = 100;
  Xorshift xs;

  sb->BarrierBlocking();
  while(true){
    if(xs()%2 == 0){
      if(ls->insert(xs()%keyrange)){
        insertcounter[id].first++;
      }else{
        insertcounter[id].second++;
      }
    }else{
      if(ls->remove(xs()%keyrange)){
        removecounter[id].first++;
      }else{
        removecounter[id].second++;
      }
    }

    if(eb->BarrierNonBlocking(SLAVE)){
      break;
    }
  }
}

void tf(){
  vector<thread> threads;
  Barrier sb(threadsnum+1, true);
  Barrier eb(threadsnum+1, false);

  test::TSXlist ls;

  for(int i=0; i<threadsnum; i++){
    threads.push_back(thread(run, i, &sb, &eb, &ls));
  }

  sb.BarrierBlocking();

  std::this_thread::sleep_for(std::chrono::seconds(1));

  eb.BarrierNonBlocking(MASTER);

  for(auto& t: threads){
    t.join();
  }

  ls.dump();
}

int main(){

  tf();

  int dif = 0;
  for(int i=0; i<threadsnum; i++){
    dif += insertcounter[i].first - removecounter[i].first;
    cout << "[" << i << "] "
         << insertcounter[i].first << " " << insertcounter[i].second << " "
         << removecounter[i].first << " " << removecounter[i].second << endl;
  }
  cout << "#node inserted " << dif << endl;

  return 0;
}
