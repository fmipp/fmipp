#include <stdarg.h>
#include <assert.h>
#include <stdio.h>

#include "fmuutil.h"

void logger(fmiComponent m, fmiString instanceName, fmiStatus status,
		   fmiString category, fmiString message, ...) 
{
	char msg[4096];
	char buf[4096];
	int len;
	int capacity;

	va_list ap;
	va_start(ap, message);
	capacity = sizeof(buf) - 1;
#if defined(_MSC_VER) && _MSC_VER>=1400
	len = _snprintf_s(msg, capacity, _TRUNCATE, "%s: %s", instanceName, message);
	if (len < 0) goto fail;
	len = vsnprintf_s(buf, capacity, _TRUNCATE, msg, ap);
	if (len < 0) goto fail;
#elif defined(WIN32)
	len = _snprintf(msg, capacity, "%s: %s", instanceName, message);
	if (len < 0) goto fail;
	len = vsnprintf(buf, capacity, msg, ap);
	if (len < 0) goto fail;
#else
	len = snprintf(msg, capacity, "%s: %s", instanceName, message);
	if (len < 0) goto fail;
	len = vsnprintf(buf, capacity, msg, ap);
	if (len < 0) goto fail;
#endif
	/* append line break */
	buf[len] = '\n';
	buf[len + 1] = 0;
	va_end(ap);

	switch (status) {
		case fmiFatal:
			printf(buf);
			break;
		default:
			printf(buf);
			break;
	}
	return;

fail:
	printf("logger failed, message too long?");
}



/**
 *  
 */
void showStatus(char* op, fmiStatus status) {
  
  printf("%s : ",op); 

  switch(status) { // @todo function, use logger ?
  
  case fmiOK: printf("well done\n"); break; 
  case fmiWarning: printf("warning: \n"); break; 
  case fmiDiscard: printf("Discard: smaller step size?\n"); break;
  case fmiError: printf("Error: error in the instance?\n"); break; 
  case fmiFatal: printf("Fatal: banik .. \n");break; 
#if defined(WIN32) and not defined(MINGW)
  case fmiPending: printf("Pending: What is the hell is that for???\n");break;
#endif
  default: assert(fmiFalse); 

  }

}


/**
 *
 */ 
void showArray(char* op,fmiReal arr[],size_t n) {
  size_t i; 

  printf("%s : ", op); 
  for(i=0;i<n;i++) 
    printf("%6.3f : ",arr[i]); 
  printf("\n");

}
