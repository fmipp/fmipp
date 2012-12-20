using namespace std; 

#define MODEL_IDENTIFIER foo
#include "FMU.h" 
#include <cstdio>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) { 
 
  string MODELNAME("foo"); 
  FMU fmu(MODELNAME);
 
  fmiStatus status = fmu.instantiate(fmiFalse);
  //  fmu.logger(status,"instantiation");
  if(status != fmiOK) cout << "instantiation : " << status << endl;

  //  status = fmu.setValue("p",2.5);
  //  status = fmu.setValue("x",.3);
  status = fmu.setValue("w",10);
  // fmu.setValue(refZ,1); this should not work at least with FMUs from other tools

  status = fmu.initialize();
  //  fmu.logger(status,"initialization");

  if(status != fmiOK)  cout << "initialization : " << status << endl;

  //  cout << "# of val refs: " << fmu.nValueRefs() << endl ;
  double t = 0;
  double tstop = 10.0; 
  double commStepSize = 1e-8;
  double x, y;

  status = fmu.getValue("x",&x);
  status = fmu.getValue("y",&y);

  printf("%.3f,%.4f,%.4f\n",t,x,y);

  int ctr=0;
  while (t < tstop) {
    ctr++;
    fmu.integrate(t+commStepSize);

    status = fmu.getValue("x",&x);
    status = fmu.getValue("y",&y); 

    t += commStepSize; 
    if(!(ctr%1000000))
      printf("%.3f,%.4f,%.4f\n",t,x,y);
  }

  return 0;
}
