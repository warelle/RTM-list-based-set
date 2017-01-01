#include <limits>
#include <immintrin.h>

namespace test{

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

    void search(int val, Window* &win);

  public:
    TSXlist();
    ~TSXlist();

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
}

void TSXlist::search(int val, Window* &win){
  Node* prev = head;
  Node* curr = nullptr;

  win->found = false;
  while(curr != tail){
    curr = prev->next;

    if(curr->value >= val){
      break;
    }
    prev = curr;
  }

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
      return false;
    }

    newNode->next = win->curr;

    auto status = _xbegin();
    if(status == _XBEGIN_STARTED){
      if(win->prev->next == win->curr){
        win->prev->next = newNode;
        _xend();
        return true;
      }
      _xabort(0xff);
    }
  }
}
bool TSXlist::contain(int val){
  Window* win = new Window();
  search(val,win);

  return win->found;
}
bool TSXlist::remove(int val){
  Window* win = new Window();

  while(true){
    search(val, win);
    if(!win->found){
      return false;
    }

    auto status = _xbegin();
    if(status == _XBEGIN_STARTED){
      if(win->prev->next == win->curr
        && win->curr->next == win->succ){
        win->prev->next = win->succ;
        win->curr->next = head;
        _xend();
        return true;
      }
      _xabort(0xff);
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
