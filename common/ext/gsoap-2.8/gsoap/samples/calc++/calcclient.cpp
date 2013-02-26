/*
	calcclient.cpp

	Example calculator service client in C++

	$ soapcpp2 -i calc.h
	$ cc -o calcclient calcclient.cpp stdsoap2.cpp soapC.cpp soapcalcProxy.cpp
	where stdsoap2.cpp is in the 'gsoap' directory, or use libgsoap++:
	$ cc -o calcclient calcclient.cpp soapC.cpp soapcalcProxy.cpp -lgsoap++

--------------------------------------------------------------------------------
gSOAP XML Web services tools
Copyright (C) 2001-2008, Robert van Engelen, Genivia, Inc. All Rights Reserved.
This software is released under one of the following two licenses:
GPL or Genivia's license for commercial use.
--------------------------------------------------------------------------------
GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#include "soapcalcProxy.h"
#include "calc.nsmap"

const char server[] = "http://websrv.cs.fsu.edu/~engelen/calcserver.cgi";

int main(int argc, char **argv)
{ if (argc < 4)
  { fprintf(stderr, "Usage: [add|sub|mul|div|pow] num num\n");
    exit(0);
  }
  double a, b, result;
  a = strtod(argv[2], NULL);
  b = strtod(argv[3], NULL);
  calcProxy calc;
  calc.soap_endpoint = server;
  switch (*argv[1])
  { case 'a':
      calc.add(a, b, &result);
      break;
    case 's':
      calc.sub(a, b, &result);
      break;
    case 'm':
      calc.mul(a, b, &result);
      break;
    case 'd':
      calc.div(a, b, &result);
      break;
    case 'p':
      calc.pow(a, b, &result);
      break;
    default:
      fprintf(stderr, "Unknown command\n");
      exit(0);
  }
  if (calc.error)
    calc.soap_stream_fault(std::cerr);
  else
    printf("result = %g\n", result);
  return 0;
}

