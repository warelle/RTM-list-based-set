#ifndef BARCONDH
#define BARCONDH

#include <mutex>
#include <condition_variable>

namespace barrier{

typedef int barrier_cond_type;

const barrier_cond_type MASTER = 0;
const barrier_cond_type SLAVE  = 1;

class Barrier{
  private:
    int threadnum;
    int remain;
    bool blocking;
    std::condition_variable cv;
    std::mutex mtx;
  public:
    Barrier(int n=0, bool blk=true):
      threadnum(n),blocking(blk),
      remain(n), cv(), mtx() {}
    ~Barrier(){};

    // blocking
    void BarrierBlocking();

    // non-blocking
    bool BarrierNonBlocking(barrier_cond_type bct);
};

void Barrier::BarrierBlocking(){
  std::unique_lock<std::mutex> ulk(mtx);
  remain--;
  if(remain == 0){
    cv.notify_all();
  }else{
    while(remain != 0){
      cv.wait(ulk);
    }
  }
  ulk.unlock();
}

bool Barrier::BarrierNonBlocking(barrier_cond_type bct){
  // put here to work even if optimizing takes place
  mtx.lock();
  if(bct == MASTER){
    remain = 0;
  }
  mtx.unlock();

  if(remain == 0) return true;
  if(bct != MASTER) return false;

  return true;
}


};

#endif
