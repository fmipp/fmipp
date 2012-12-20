using namespace std; 

#include "IncrementalFMU.h" 
#include <cstdio>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) { 
  std::string foo[2] = { "k", "x" };
  double bar[2] = { 10, 1 };
  double* state;
  double* result;
  double time=0.0;
  double next=100;
  double oldnext;

  std::string none[0];
  std::string foobar[2] = { "x", "z" };

  const double stepsize=0.001;
  //  const double stepsize=0.1;

  fmiReal p_;
  fmiReal x_;
  fmiReal z_;
 
  //  cout << "creating new IncrementalFMU \"Events\"" << endl;

  IncrementalFMU fmu("Events", none, 0, foobar, 2);
 
  // cout << "calling fmu.init(foo, bar, 2, 0, 0.1, 0.1)" << endl;
  fmu.init("Events1", foo, bar, 2, 0, 2*stepsize, stepsize);

  //cout << "calling fmu.sync(-42, 0) \t";
  next=fmu.sync(-42,0);
  //cout << "result: ";
  state = fmu.getCurrentState();
  /*
    fmu.getValue("x",&x_);
    fmu.getValue("z",&z_); 
    cout << 0 << "," << x_ << "," << z_ << "," << state[0] << endl;
  */
  result = fmu.getCurrentOutputs();
  cout << 0 << "," << result[0] << "," << result[1] << "," << state[0] << endl;

  for(int k = 0; k < 1; k++) {
    //    cout << "state " << k << ": " << result[k] << endl;
  }

  time = 0;
  //  for(int j = 0; j < 10000; j++) {
  while(time < 1) {
    //    cout << "calling fmu.sync(" << (double)j/10 << ", " << (double)(j+1)/10 << ") \t";
    //    time = fmu.sync((double)j/10000, (double)(j+1)/10000);
    oldnext = next;
    next = fmu.sync(time, (time+stepsize)>next?next:(time+stepsize));
    /*
    if(next < time + 0.02) {
      double foobar = next;
      next = fmu.sync(time, next);
      time = foobar;
    } else {
      next = fmu.sync(time, time+0.02);
      time += 0.02;
    }
    */

    //cout << time <<endl;
    //    cout << "result: ";
    //    result = fmu.getCurrentState();
    /*
    for(int k = 0; k < 1; k++) {
      cout << "state " << k << ": " << result[k];
    }
    while(time < (double)(j+1)/1000) {
      time = fmu.sync(time, (double)(j+1)/1000);
    }
    */
    state = fmu.getCurrentState();
    /*
    fmu.getValue("x",&x_);
    fmu.getValue("z",&z_); 
    */
    //    cout << (double)(j+1)/1000 << "," << x_ << "," << z_ << "," << result[0] << endl;
    //    cout << time << "," << x_ << "," << z_ << "," << state[0] << endl;
    result = fmu.getCurrentOutputs();
    cout << time << "," << result[0] << "," << result[1] << "," << state[0] << endl;
    time = (time+stepsize)>oldnext?oldnext:(time+stepsize);
  }

  /*

  size_t  nevents = fmu.nEventInds(); 
  fmiReal* eventsind; 
  eventsind = new fmiReal[nevents];

  double t = 0;
  double commStepSize = 0.1; 
  double tstop = 10.0; 

  status = fmu.getEventIndicators(eventsind);

  printf("# of events :  %d\n",nevents);
  printf("  time      x        z        ev1\n");
  printf("%6.3f %8.4f  %8.4f  %8.4f\n",t,x_,z_,eventsind[0]);

  while (t < tstop) {
    fmu.integrate(t+commStepSize);

    status = fmu.getValue("x",&x_);
    status = fmu.getValue("z",&z_); 
    status = fmu.getEventIndicators(eventsind);

    t += commStepSize; 
    printf("%6.3f %8.4f  %8.4f  %8.4f\n",t,x_,z_,eventsind[0]);
  }

  cout << "time " << t << endl; 
  delete[] eventsind; 
  */
  return 0; 

}
