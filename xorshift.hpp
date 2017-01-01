#ifndef XORSHIFTH
#define XORSHIFTH

#include <random>

class Xorshift{
  private:
    unsigned x=123456789u;
    unsigned y=362436069u;
    unsigned z=521288629u;
    unsigned w=0;
    unsigned random(){
      unsigned t;
      t = x^(x << 11);
      x = y; y = z; z = w;
      w = (w^(w >> 19))^(t^(t >> 8));
      return w;
    }
  public:
    unsigned operator()(){
      return random();
    }
    Xorshift(){
      std::random_device rd;
      w = rd();
    }
    ~Xorshift(){}
};

#endif
