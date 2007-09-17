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

#ifndef COMPRESSED_STREAM_HPP
#define COMPRESSED_STREAM_HPP

#include <vector>
#include <iostream>
#include <algorithm>

namespace compressed_stream{
namespace property{

	template<
	typename Elem, 
	typename Tr = std::char_traits<Elem> 
>	
class ostream_property
{
public:
	typedef std::basic_ostream<Elem, Tr>& ostream_reference;

	explicit ostream_property(
		ostream_reference ostream_
		)
	: m_ostream(ostream_)
	{};


	ostream_reference get_ostream() const	{	return m_ostream;};
private:
	ostream_reference m_ostream;
};

template<
	typename Elem, 
	typename Tr = std::char_traits<Elem> 
>	
class istream_property
{
public:
	typedef std::basic_istream<Elem, Tr>& ostream_reference;

	explicit istream_property(
		istream_reference istream_
		)
	: m_istream(istream_)
	{};


	istream_reference get_istream() const	{	return m_istream;};
private:
	istream_reference m_istream;
};


}; // property namespace

template<
	class CompressorPolicy,
	class Elem, 
	class Tr = std::char_traits<Elem> 
>	
class basic_compressor_streambuf : 
	public std::basic_streambuf<Elem, Tr>,
	public property::ostream_property<Elem,Tr>
{
public:
	typedef std::basic_streambuf<Elem, Tr>  basic_streambuf_type;
	typedef property::ostream_property<Elem,Tr> ostream_property_type;

	typedef CompressorPolicy compressor_type;
	typedef CompressorPolicy& compressor_reference;
	typedef CompressorPolicy const& compressor_const_reference;

	typedef unsigned char byte_type;
	typedef byte_type* byte_buffer_type;
	typedef std::vector<byte_type> byte_vector_type;
	typedef std::vector<char_type> char_vector_type;

    explicit basic_compressor_streambuf(
		ostream_reference ostream_,
		size_t buffer_size_
		);
	
	~basic_compressor_streambuf();

	int sync ();
    int_type overflow (int_type c);
	std::streamsize flush();

	compressor_reference get_compressor()				{	return m_compressor;};
	compressor_const_reference get_compressor() const	{	return m_compressor;};
private:
	compressor_type m_compressor;
};

template<
	class DecompressorPolicy,
	class Elem, 
	class Tr = std::char_traits<Elem> 
>
class basic_decompressor_streambuf : 
	public std::basic_streambuf<Elem, Tr>,
	public property::istream_property<Elem,Tr>
{
public:
	typedef std::basic_streambuf<Elem, Tr> basic_streambuf_type;
	typedef property::istream_property<Elem,Tr> istream_property_type;

	typedef DecompressorPolicy decompressor_type;
	typedef DecompressorPolicy& decompressor_reference;
	typedef DecompressorPolicy const& decompressor_const_reference;

	typedef unsigned char byte_type;
	typedef byte_type* byte_buffer_type;
	typedef std::vector<byte_type> byte_vector_type;
	typedef std::vector<char_type> char_vector_type;

    explicit basic_decompressor_streambuf(
		istream_reference istream_,
		size_t window_size_,
		size_t read_buffer_size_,
		size_t input_buffer_size_
		);
	
	~basic_decompressor_streambuf();

    int_type underflow();

	decompressor_reference get_decompressor()				{	return m_decompressor;};
	decompressor_const_reference get_decompressor() const	{	return m_decompressor;};
private:
	decompressor_type m_decompressor;
};

template<
	class Elem, 
	class Tr = std::char_traits<Elem> 
>	
class basic_compressor_ostreambase : virtual public std::basic_ios<Elem,Tr>
{
public:
	typedef std::basic_ostream<Elem, Tr>& ostream_reference;
	typedef basic_compressor_streambuf<Elem,Tr> compressor_streambuf_type;

	explicit basic_compressor_ostreambase( 
		ostream_reference ostream_,
		size_t level_,
		EStrategy strategy_,
		size_t window_size_,
		size_t memory_level_,
		size_t buffer_size_
		)
		: m_buf(ostream_,level_,strategy_,window_size_,memory_level_,buffer_size_)
	{
		init(&m_buf );
	};
	
	compressor_streambuf_type* rdbuf() { return &m_buf; };

private:
	compressor_streambuf_type m_buf;
};

template<
	class Elem, 
	class Tr = std::char_traits<Elem> 
>	
class basic_zip_istreambase : virtual public std::basic_ios<Elem,Tr>
{
public:
	typedef std::basic_istream<Elem, Tr>& istream_reference;
	typedef basic_decompressor_streambuf<Elem,Tr> unzip_streambuf_type;

	explicit basic_zip_istreambase( 
		istream_reference ostream_,
		size_t window_size_,
		size_t read_buffer_size_,
		size_t input_buffer_size_
		)
		: m_buf(ostream_,window_size_, read_buffer_size_, input_buffer_size_)
	{
		init(&m_buf );
	};
	
	unzip_streambuf_type* rdbuf() { return &m_buf; };

private:
	unzip_streambuf_type m_buf;
};

template<
	class Elem, 
	class Tr = std::char_traits<Elem> 
>	
class basic_compressor_ostream : 
	public basic_compressor_ostreambase<Elem,Tr>, 
	public std::basic_ostream<Elem,Tr>
{
public:
	typedef basic_compressor_ostreambase<Elem,Tr> zip_ostreambase_type;
	typedef std::basic_ostream<Elem,Tr> ostream_type;

	explicit basic_compressor_ostream( 
		ostream_reference ostream_, 
		int open_mode = std::ios::out, 
		size_t level_ = Z_DEFAULT_COMPRESSION,
		EStrategy strategy_ = DefaultStrategy,
		size_t window_size_ = 15,
		size_t memory_level_ = 8,
		size_t buffer_size_ = default_buffer_size
		)
	: 
		zip_ostreambase_type(ostream_,level_,strategy_,window_size_,memory_level_,buffer_size_), 
		ostream_type(rdbuf(),open_mode )
	{

	};
	
	basic_compressor_ostream<Elem,Tr>& add_header();
	basic_compressor_ostream<Elem,Tr>& zflush()	
	{	
		flush(); rdbuf()->flush(); return *this; 
	};
};

template<
	class Elem, 
	class Tr = std::char_traits<Elem> 
>	
class basic_zip_istream : 
	public basic_zip_istreambase<Elem,Tr>, 
	public std::basic_istream<Elem,Tr>
{
public:
	typedef basic_zip_istreambase<Elem,Tr> zip_istreambase_type;
	typedef std::basic_istream<Elem,Tr> istream_type;
	typedef unsigned char byte_type;

	explicit basic_zip_istream( 
		istream_reference istream_, 
		int open_mode = std::ios::in,
		size_t window_size_ = 15,
		size_t read_buffer_size_ = default_buffer_size,
		size_t input_buffer_size_ = default_buffer_size
		)
	  : 
		zip_istreambase_type(istream_,window_size_, read_buffer_size_, input_buffer_size_), 
		istream_type(rdbuf(), open_mode )
	{
 	      if (rdbuf()->get_zerr()==Z_OK)
			  check_header();
	};

protected:
	int check_header();
};

typedef basic_compressor_ostream<char> zip_ostream;
typedef basic_compressor_ostream<wchar_t> zip_wostream;
typedef basic_zip_istream<char> zip_istream;
typedef basic_zip_istream<wchar_t> zip_wistream;

}; // zlib_sream

#include "zipstream.ipp"

#endif

