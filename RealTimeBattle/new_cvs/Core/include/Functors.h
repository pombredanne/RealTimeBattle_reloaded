#ifndef __FUNCTORS__
#define __FUNCTORS__

class RandomFunction {
public:
  RandomFunction( double time ) : time( time ) {}
  virtual ~RandomFunction() {};
  virtual bool can_exec( double timestep ) {
    return ((double)rand()) / (double)RAND_MAX <= timestep * time ;
  }
  
  virtual void do_exec() = 0;
protected:
  double time;
  //virtual double next_exec() { return 0; }
};


#endif


