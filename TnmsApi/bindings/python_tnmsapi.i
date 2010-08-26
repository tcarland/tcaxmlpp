%module python_tnmsapi
%{
/* Includes the header in the wrapper code */
#include "TnmsAPI.h"
%}

%feature("autodoc", "1");

typedef long time_t;
/* Parse the header file to generate wrappers */
%include "TnmsAPI.h"
