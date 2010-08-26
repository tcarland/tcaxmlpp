%module perl_tnmsapi
%{
/* Includes the header in the wrapper code */
#include "TnmsAPI.h"
%}

typedef long time_t;
/* Parse the header file to generate wrappers */
%include "TnmsAPI.h"
