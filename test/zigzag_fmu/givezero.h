#ifdef _MSC_VER
#define DllExport __declspec( dllexport )
#else
#define DllExport
#endif

DllExport int giveZero( void );