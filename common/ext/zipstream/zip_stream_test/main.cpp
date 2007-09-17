/*
zipstream Library License:
--------------------------

The zlib/libpng License Copyright (c) 2003 Jonathan de Halleux.

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution

Author: Jonathan de Halleux, dehalleux@pelikhan.com, 2003
*/

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cstdlib>

//#include "bzip2_stream_test.hpp"
#include "zip_stream_test.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    std::cout<<"--------------------------------------------------"<<endl;
    std::cout<<"----------------------zlib------------------------"<<endl;
    std::cout<<"--------------------------------------------------"<<endl;
	zlib_stream::test_buffer_to_buffer();
	zlib_stream::test_wbuffer_to_wbuffer();
	zlib_stream::test_string_string();
	zlib_stream::test_wstring_wstring();
	zlib_stream::test_file_file(false);
	zlib_stream::test_file_file(true);
	zlib_stream::test_crc();

/*
    std::cout<<"--------------------------------------------------"<<endl;
    std::cout<<"----------------------bz2-------------------------"<<endl;
    std::cout<<"--------------------------------------------------"<<endl;
    bzip2_stream::test_buffer_to_buffer();
	bzip2_stream::test_wbuffer_to_wbuffer();
	bzip2_stream::test_string_string();
	bzip2_stream::test_wstring_wstring();
	bzip2_stream::test_file_file();
*/
	//system("pause");
	return 0;
}
