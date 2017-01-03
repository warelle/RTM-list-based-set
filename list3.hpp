#include <limits>
#include <immintrin.h>
#include "split.hpp"

namespace test{

using namespace tmsplit;

const int OP_SEARCH = 0;
const int OP_INSERT = 1;
const int OP_REMOVE = 2;

struct Node{
  int value;
  Node* next;

  Node(int val):value(val),next(nullptr){}
};

struct Window{
  Node *prev;
  Node *curr;
  Node *succ;
  bool found;
};

class TSXlist{
  private:
    Node* head;
    Node* tail;

    TMsplit<3>* tmsplit;

    void search(int val, Window* &win);

  public:
    TSXlist();
    ~TSXlist();

    void ThreadInit();

    bool insert(int val);
    bool contain(int val);
    bool remove(int val);

    // debug
    void dump();
};

TSXlist::TSXlist(){
  head = new Node(std::numeric_limits<int>::min());
  tail = new Node(std::numeric_limits<int>::max());
  head->next = tail;
  tail->next = nullptr;

  // tmp arg: #op
  // 1st arg: #thread
  tmsplit = new TMsplit<3>(4);

//  // 0~99 initial list
//  Node* runner = head;
//  for(int i=0; i<100; i++){
//    Node* n = new Node(i);
//    runner->next = n;
//    runner = n;
//  }
//  runner->next = tail;
}
TSXlist::~TSXlist(){
  delete head;
  delete tail;

  delete tmsplit;
}

void TSXlist::ThreadInit(){
  tmsplit->ThreadRegister();
}

void TSXlist::search(int val, Window* &win){
  Node* prev = nullptr;
  Node* curr = nullptr;

retry:;
  tmsplit->Init(OP_SEARCH);
  tmsplit->Start();

  prev = head;
  curr = head;

  win->found = false;
  while(curr != tail){
    curr = prev->next;

    if(curr == nullptr){
      tmsplit->Commit();
      goto retry;
    }

    if(curr->value >= val){
      break;
    }

    prev = curr;

    tmsplit->CheckPoint();
  }
  tmsplit->Commit();

  win->curr = curr;
  if(curr->value == val){
    win->found = true;
  }
  win->prev = prev;
  win->curr = curr;
  win->succ = curr->next;
}

bool TSXlist::insert(int val){
  Window* win = new Window();
  Node* newNode = new Node(val);

  while(true){
    search(val, win);
    if(win->found){
      delete newNode;
      delete win;
      return false;
    }

    newNode->next = win->curr;

    tmsplit->Init(OP_INSERT);
    tmsplit->Start();

    if(win->prev->next == win->curr){
      win->prev->next = newNode;
      tmsplit->Commit();

      delete win;
      return true;
    }else{
      // failure
      tmsplit->Commit();
    }
  }
}
bool TSXlist::contain(int val){
  Window* win = new Window();
  search(val,win);

  bool found = win->found;
  delete win;

  return found;
}
bool TSXlist::remove(int val){
  Window* win = new Window();

  while(true){
    search(val, win);
    if(!win->found){
      delete win;
      return false;
    }

    tmsplit->Init(OP_REMOVE);
    tmsplit->Start();

    if(win->prev->next == win->curr
        && win->curr->next == win->succ
        && win->succ != nullptr){

      win->prev->next = win->succ;
      win->curr->next = nullptr;

      tmsplit->Commit();

      delete win;
      return true;
    }else{
      tmsplit->Commit();
    }
  }
}

void TSXlist::dump(){
  Node* runner = head->next;
  int sizecount = 0;

  while(runner != tail){
    std::cout << runner->value << " ";
    runner = runner->next;
    sizecount++;
  }
  std::cout << std::endl << "#nodes " << sizecount << std::endl;
}

};
