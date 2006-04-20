%module TACOStates
%include <std_string.i>

%{
// Python.h defines OVERFLOW
#undef OVERFLOW

#include <TACOStates.h>
%}

%include <TACOStates.h>
