#ifndef TMSPLITH
#define TMSPLITH

#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <algorithm>
#include <immintrin.h>

namespace tmsplit{

typedef unsigned int TMSTATUS;
typedef std::vector<int> PERTHREAD;

static std::atomic<int> threadid_;
thread_local int threadid;

// [reference]
//   http://stackoverflow.com/questions/13193484/how-to-declare-a-vector-of-atomic-in-c
template <typename T>
struct atomWrapper{
  std::atomic<T> _a;

  atomWrapper():_a(){}
  atomWrapper(const std::atomic<T> &a):_a(a.load()){}
  atomWrapper(const atomWrapper<T> &other):_a(other._a.load()){}
  atomWrapper &operator=(const atomWrapper<T> &other){
    _a.store(other._a.load());
  }
};

// N: #operation
template<int N>
class TMsplit{
  private:
    std::vector<std::thread::id> tid;
    std::mutex mtx;
    int GetThreadId();

    PERTHREAD opid;
    PERTHREAD splits;
    PERTHREAD steps;
    PERTHREAD limit;

    std::array<std::vector<atomWrapper<int>>, N> limits;

  public:
    TMsplit(int threadnum);
    ~TMsplit();

    void ThreadRegister();

    void Init(int operationid);
    void Start();
    void CheckPoint();
    void Commit();
    void ManageSplitCommit();
    void ManageSplitAbort();
};

template<int N>
TMsplit<N>::TMsplit(int threadnum){
//  for(auto& i: limits){
//    i = std::vector<atomWrapper<int>>(N,atomWrapper<int>(5));
//  }
  opid   = PERTHREAD(threadnum, 0);
  splits = PERTHREAD(threadnum, 0);
  steps  = PERTHREAD(threadnum, 0);
  limit  = PERTHREAD(threadnum, 0);
}
template<int N>
TMsplit<N>::~TMsplit(){
}

template<int N>
void TMsplit<N>::ThreadRegister(){
  threadid = threadid_.fetch_add(1);
}
template<int N>
int TMsplit<N>::GetThreadId(){
  return threadid;
}

template<int N>
void TMsplit<N>::Init(int operationid){
  int id = GetThreadId();
  splits[id] = 0;
  opid[id]   = operationid;
}

template<int N>
void TMsplit<N>::Start(){
  int id = GetThreadId();
  steps[id] = 0;
  //limit[id] = limits[opid][splits];
  limit[id] = 100;
  while(true){
    TMSTATUS status = _xbegin();
    if(status == _XBEGIN_STARTED){
      break;
    }

    ManageSplitAbort();
    steps[id]= 0;
    limit[id]--;
    //limit[id] = limits[opid][splits];
  }
}

template<int N>
void TMsplit<N>::CheckPoint(){
  int id = GetThreadId();
  steps[id]++;

  if(steps[id] >= limit[id]){
    Commit();
    Start();
  }
}

template<int N>
void TMsplit<N>::Commit(){
  int id = GetThreadId();
  splits[id]++;

  _xend();
  ManageSplitCommit();
}

template<int N>
void TMsplit<N>::ManageSplitCommit(){
  // currently no work
}
template<int N>
void TMsplit<N>::ManageSplitAbort(){
  // currently no work
}

};

#endif
