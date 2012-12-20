using namespace std; 

#define MODEL_IDENTIFIER Events

#include "FMU.h" 
#include <cstdio>
#include <cstdlib>
#include <iostream>

/** no need for this part any more .. */ 
#if defined(DYMOLA)
  enum ValueReference { refK = -1, refX = -1, refZ = -1 }; // Dymola.
#elif defined(OPENMODELICA)
  enum ValueReference { refK = 3, refX = 0, refZ = 2 }; // OpenModelica.
#endif


int main(int argc, char** argv) { 

 
  string MODELNAME("Events"); 
  FMU fmu(MODELNAME);
 
  fmiStatus status = fmu.instantiate("Events1", fmiFalse);
  //  fmu.logger(status,"instantiation");
  //  if(status != fmiOK) cout << "instantiation : " << status << endl;

  //  status = fmu.setValue("k",1.5);
  //  status = fmu.setValue("x",.1);
  status = fmu.setValue("k",10);
  status = fmu.setValue("x",1);
  // fmu.setValue(refZ,1); this should not work at least with FMUs from other tools

  status = fmu.initialize();
  //  fmu.logger(status,"initialization");

  //  if(status != fmiOK)  cout << "initialization : " << status << endl;

  //  cout << "# of cont vars: " << fmu.nValueRefs() << endl ;

  fmiReal p_;
  fmiReal x_;
  fmiReal z_;

  size_t  nevents = fmu.nEventInds(); 
  fmiReal* eventsind; 
  eventsind = new fmiReal[nevents];

  double t = 0;
  double commStepSize = 0.0001; 
  double tstop = 1.0; 

  status = fmu.getValue("x",&x_);
  status = fmu.getValue("z",&z_); 
  status = fmu.getEventIndicators(eventsind);

  //  printf("# of events :  %d\n",nevents);
  //  printf("  time      x        z        ev1\n");
  printf("%6.3f,%8.4f,%8.4f,%8.4f\n",t,x_,z_,eventsind[0]);

  while (t < tstop) {
    fmu.integrate(t+commStepSize);

    status = fmu.getValue("x",&x_);
    status = fmu.getValue("z",&z_); 
    status = fmu.getEventIndicators(eventsind);

    t += commStepSize; 
    printf("%6.3f,%8.4f,%8.4f,%8.4f\n",t,x_,z_,eventsind[0]);
  }

  //  cout << "time " << t << endl; 
  delete[] eventsind; 

  return 0; 

}
