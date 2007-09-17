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

#include "bzip2stream.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <boost/crc.hpp>

using namespace std;
namespace bzip2_stream{

// a dummy class
class dummy
{
public:
	dummy(float f_ = 3.14, int i_= 1) : f(f_), i(i_)
	{};
	void reset(){f=0;i=0;};

	friend ostream& operator << ( ostream& out, dummy const& d)
	{
		out<<" "<<d.f<<" "<<d.i;
		return out;
	}
	friend istream& operator >> ( istream& in, dummy& d)
	{
		in>>d.f>>d.i;
		return in;
	}

	friend wostream& operator << ( wostream& out, dummy const& d)
	{
		out<<L" "<<d.f<<L" "<<d.i;
		return out;
	}
	friend wistream& operator >> ( wistream& in, dummy& d)
	{
		in>>d.f>>d.i;
		return in;
	}
protected:
	float f;
	int i;
};

void test_buffer_to_buffer()
{
	boost::crc_16_type crc_in,crc_zip;
	const size_t n= 1024;
	char in_buffer[n]={'\0'};
	char zip_buffer[n]={'\0'};

	for (size_t i=0;i<n-1;++i)
		in_buffer[i]=static_cast<char>(48+i%48);


	ostringstream out;
	bzip2_ostream zipper(out);

	zipper.write(in_buffer, n);
	if (zipper.fail())
		cerr<<"failed to write stream"<<endl;

	zipper.zflush();


	istringstream in( out.str() );
	bzip2_istream unzipper( in);
	unzipper.read(zip_buffer, n);
	crc_in.process_bytes(in_buffer,n);
	crc_zip.process_bytes(zip_buffer,n);
	cerr<<"buffers equals: "<<endl
		<<"-----------------"<<endl
		<<"\t crc source:"<<crc_in.checksum()<<endl
		<<"\t crc result:"<<crc_zip.checksum()<<endl;

}

void test_wbuffer_to_wbuffer()
{
	boost::crc_16_type crc_in,crc_zip;
	const size_t n= 128;
	wchar_t in_buffer[n]={'\0'};
	wchar_t zip_buffer[n]={'\0'};

	for (size_t i=0;i<n-1;++i)
		in_buffer[i]=static_cast<wchar_t>(48+i%48);


	wostringstream out;
	bzip2_wostream zipper(out);

	zipper.write(in_buffer, n);
	if (zipper.fail())
		cerr<<"failed to write stream"<<endl;

	zipper.zflush();


	wistringstream in( out.str() );
	bzip2_wistream unzipper( in);
	unzipper.read(zip_buffer, n);

	crc_in.process_bytes(in_buffer,n*sizeof(wchar_t));
	crc_zip.process_bytes(zip_buffer,n*sizeof(wchar_t));
	wcerr<<L"buffers equals: "<<endl
		<<L"-----------------"<<endl
		<<L"\t crc source:"<<crc_in.checksum()<<endl
		<<L"\t crc result:"<<crc_zip.checksum()<<endl;
}

void test_string_string()
{
	// create some test values
	char c_r='-',c= 'a';
	string s_r="",s = "std::string";
	double d_r=0,d = asin( 1.0 ) *2.0;
	float f_r=0, f = static_cast<float>(asin( 1.0 ) *2.0);
	unsigned int ui_r=0,ui = rand();
	unsigned long ul_r=0,ul = rand();
	unsigned short us_r=0, us = rand();
	dummy dum,dum_r(0,0);

	/*----------------------------------------------------------------------
	
	Testing string to string zipping/unzipping
	
	-------------------------------------------------------------------------*/
	// creating the target zip string, could be a fstream
	ostringstream ostringstream_;
	// creating the zip layer
	bzip2_ostream zipper(ostringstream_);

	// writing data
	zipper<<f<<" "<<d<<" "<<ui<<" "<<ul<<" "<<us<<" "<<c<<" "<<dum;
	// zip ostream needs special flushing...
	zipper.zflush();

	// create a stream on zip string
	istringstream istringstream_( ostringstream_.str());
	// create unzipper istream
	bzip2_istream unzipper( istringstream_);

	// unzipping
	unzipper>>f_r>>d_r>>ui_r>>ul_r>>us_r>>c_r>>dum_r;

	// ouputing results
	cerr<<"tests string-string,  char:"<<endl
		<<"----------------------------"<<endl
		<<"double : "<<d<<" "<<d_r<<endl
		<<"char : "<<c<<" "<<c_r<<endl
		<<"float : "<<f<<" "<<f_r<<endl
		<<"unsigned int : "<<ui<<" "<<ui_r<<endl
		<<"unsigned long : "<<ul<<" "<<ul_r<<endl
		<<"unsigned short : "<<us<<" "<<us_r<<endl
		<<"dummy class: "<<dum<<" "<<dum_r<<endl
		<<endl
		;
}

void test_wstring_wstring()
{
	// create some test values
	char c_r='-',c= 'a';
	double d_r=0,d = asin( 1.0 ) *2.0;
	float f_r=0, f = static_cast<float>(asin( 1.0 ) *2.0);
	unsigned int ui_r=0,ui = rand();
	unsigned long ul_r=0,ul = rand();
	unsigned short us_r=0, us = rand();
	dummy dum,dum_r(0,0);

	/*

	Testing wide characters

	*/
	f_r=0;
	d_r=0;
	ui_r=ul_r=us_r=0;
	dum_r.reset();
	// creating the target zip string, could be a fstream
	wostringstream wostringstream_;
	// creating the zip layer
	bzip2_wostream wzipper(wostringstream_);

	// writing data
	wzipper<<f<<L" "<<d<<L" "<<ui<<L" "<<ul<<L" "<<us<<L" "<<dum;
	// zip ostream needs special flushing...
	wzipper.zflush();


	// create a stream on zip string
	wistringstream wistringstream_( wostringstream_.str());
	// create unzipper istream
	bzip2_wistream wunzipper( wistringstream_);

	// unzipping
	wunzipper>>f_r>>d_r>>ui_r>>ul_r>>us_r>>dum_r;

	// ouputing results
	cerr<<"tests string-string (wchar_t):"<<endl
		<<"------------------------------"<<endl
		<<"double : "<<d<<" "<<d_r<<endl
		<<"float : "<<f<<" "<<f_r<<endl
		<<"unsigned int : "<<ui<<" "<<ui_r<<endl
		<<"unsigned long : "<<ul<<" "<<ul_r<<endl
		<<"unsigned short : "<<us<<" "<<us_r<<endl
		<<"dummy class: "<<dum<<" "<<dum_r<<endl
		<<endl
		;
}

void test_file_file()
{
	// create some test values
	char c_r='-',c= 'a';
	const char* sz = "const char*";
	string s_r="",s = "std::string";
	double d_r=0,d = asin( 1.0 ) *2.0;
	float f_r=0, f = static_cast<float>(asin( 1.0 ) *2.0);
	unsigned int ui_r=0,ui = rand();
	unsigned long ul_r=0,ul = rand();
	unsigned short us_r=0, us = rand();
	dummy dum,dum_r(0,0);
	char sbuf[256]={'-'};

	/*----------------------------------------------------------------------------
	
	Testing file to file unzipping

	------------------------------------------------------------------------------*/
	f_r=0;
	d_r=0;
	ui_r=ul_r=us_r=0; dum_r.reset();

	{
		// creating the target zip string, could be a fstream
		ofstream ofstream_("test.bz",ios::out | ios::binary );
		// creating the zip layer
		bzip2_ostream fzipper(ofstream_);

		// writing data
		fzipper<<f<<" "<<d<<" "<<ui<<" "<<ul<<" "<<us<<" "<<c<<" "<<dum;
		// zip ostream needs special flushing...
		fzipper.zflush();
	}

	// create a stream on zip string
	ifstream ifstream_;
	ifstream_.open("test.bz", ios::in | ios::binary);
	if (!ifstream_.is_open())
	{
		cerr<<"Could not open file test.zip"<<endl;
	}
	// create unzipper istream
	bzip2_istream funzipper( ifstream_);

	// unzipping
	funzipper>>f_r>>d_r>>ui_r>>ul_r>>us_r>>c_r>>dum_r;

	// ouputing results
	cerr<<"tests file-to-file :"<<endl
		<<"------------------------------"<<endl
		<<"double : "<<d<<" "<<d_r<<endl
		<<"char : "<<c<<" "<<c_r<<endl
		<<"float : "<<f<<" "<<f_r<<endl
		<<"unsigned int : "<<ui<<" "<<ui_r<<endl
		<<"unsigned long : "<<ul<<" "<<ul_r<<endl
		<<"unsigned short : "<<us<<" "<<us_r<<endl
		<<"dummy class: "<<dum<<" "<<dum_r<<endl
		<<endl
		;
}

};