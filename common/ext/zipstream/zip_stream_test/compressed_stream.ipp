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
#ifndef ZIPSTREAM_IPP
#define ZIPSTREAM_IPP

#include "zipstream.hpp"

namespace zlib_stream{

namespace detail{
	const int gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */

	/* gzip flag byte */
	const int gz_ascii_flag =  0x01; /* bit 0 set: file probably ascii text */
	const int gz_head_crc    = 0x02; /* bit 1 set: header CRC present */
	const int gz_extra_field = 0x04; /* bit 2 set: extra field present */
	const int gz_orig_name  =  0x08; /* bit 3 set: original file name present */
	const int gz_comment    =  0x10; /* bit 4 set: file comment present */
	const int gz_reserved   =  0xE0; /* bits 5..7: reserved */	
}

	template<class Elem, class Tr>
	basic_zip_streambuf<Elem,Tr>::basic_zip_streambuf(
		ostream_reference ostream_,
		size_t level_,
		EStrategy strategy_,
		size_t window_size_,
		size_t memory_level_,
		size_t buffer_size_
	)
	: 
		m_ostream(ostream_),
		m_output_buffer(buffer_size_,0),
		m_buffer(buffer_size_,0)
	{
		m_zip_stream.zalloc=(alloc_func)0;
		m_zip_stream.zfree=(free_func)0;

		m_zip_stream.next_in=NULL;
		m_zip_stream.avail_in=0;
		m_zip_stream.avail_out=0;
		m_zip_stream.next_out=NULL;

		m_err=deflateInit2(
			&m_zip_stream, 
			std::min( 9, static_cast<int>(level_)),
			Z_DEFLATED,
			static_cast<int>(window_size_),
			std::min( 9, static_cast<int>(memory_level_) ),
			static_cast<int>(strategy_)
			);

		setp( &(m_buffer[0]), &(m_buffer[m_buffer.size()-1]));
	};

	template<class Elem, class Tr>
	basic_zip_streambuf<Elem,Tr>::~basic_zip_streambuf()
	{
		flush();
		m_ostream.flush();
		m_err=deflateEnd(&m_zip_stream);
	};

	template<class Elem, class Tr>
	int basic_zip_streambuf<Elem,Tr>::sync ()
	{ 
		if ( pptr() && pptr() > pbase()) 
		{
			int c = overflow( EOF);

			if ( c == EOF)
				return -1;
        }

        return 0;
	}

    template<class Elem, class Tr>
	typename basic_zip_streambuf<Elem,Tr>::int_type basic_zip_streambuf<Elem,Tr>::overflow (
			typename basic_zip_streambuf<Elem,Tr>::int_type c
			)
	{ 
        int w = static_cast<int>(pptr() - pbase());
        if (c != EOF) {
             *pptr() = c;
             ++w;
         }
         if ( zip_to_stream( pbase(), w)) {
             setp( pbase(), epptr() - 1);
             return c;
         } else
             return EOF;
	}

	template<class Elem, class Tr>
	bool basic_zip_streambuf<Elem,Tr>::zip_to_stream( 
		typename basic_zip_streambuf<Elem,Tr>::char_type* buffer_, 
		std::streamsize buffer_size_
		)
	{
		std::streamsize written_byte_size=0, total_written_byte_size = 0;

		m_zip_stream.next_in=(byte_buffer_type)buffer_;
		m_zip_stream.avail_in=static_cast<uInt>(buffer_size_*sizeof(char_type));
		m_zip_stream.avail_out=static_cast<uInt>(m_output_buffer.size());
		m_zip_stream.next_out=&(m_output_buffer[0]);
		size_t remainder=0;

		do
		{
			m_err = deflate(&m_zip_stream, 0);
	
			if (m_err == Z_OK  || m_err == Z_STREAM_END)
			{
				written_byte_size= static_cast<std::streamsize>(m_output_buffer.size()) - m_zip_stream.avail_out;
				total_written_byte_size+=written_byte_size;
				// ouput buffer is full, dumping to ostream
				m_ostream.write( 
					(const char_type*) &(m_output_buffer[0]), 
					static_cast<std::streamsize>( written_byte_size/sizeof(char_type) )
					);
														
				// checking if some bytes were not written.
				if ( (remainder = written_byte_size%sizeof(char_type))!=0)
				{
					// copy to the beginning of the stream
					memcpy(
						&(m_output_buffer[0]), 
						&(m_output_buffer[written_byte_size-remainder]),
						remainder);
					
				}
				
				m_zip_stream.avail_out=static_cast<uInt>(m_output_buffer.size()-remainder);
				m_zip_stream.next_out=&m_output_buffer[remainder];
			}
		} 
		while (m_zip_stream.avail_in != 0 && m_err == Z_OK);
	
		return m_err == Z_OK;
	};

	template<class Elem, class Tr>
	std::streamsize basic_zip_streambuf<Elem,Tr>::flush()
	{
		std::streamsize written_byte_size=0, total_written_byte_size=0;

		size_t remainder=0;

		do
		{
			m_err = deflate(&m_zip_stream, Z_FINISH);
			if (m_err == Z_OK || m_err == Z_STREAM_END)
			{
				written_byte_size=static_cast<std::streamsize>(m_output_buffer.size()) - m_zip_stream.avail_out;
				total_written_byte_size+=written_byte_size;
				// ouput buffer is full, dumping to ostream
				m_ostream.write( 
					(const char_type*) &(m_output_buffer[0]), 
					static_cast<std::streamsize>( written_byte_size/sizeof(char_type)*sizeof(char) )
					);
				
				// checking if some bytes were not written.
				if ( (remainder = written_byte_size%sizeof(char_type))!=0)
				{
					// copy to the beginning of the stream
					memcpy(
						&(m_output_buffer[0]), 
						&(m_output_buffer[written_byte_size-remainder]),
						remainder);
					
				}
				
				m_zip_stream.avail_out=static_cast<uInt>(m_output_buffer.size()-remainder);
				m_zip_stream.next_out=&m_output_buffer[remainder];
			}
		} while (m_err == Z_OK);

		m_ostream.flush();

		return total_written_byte_size;
	};

    template<class Elem, class Tr>
    basic_unzip_streambuf<Elem,Tr>::basic_unzip_streambuf(
		istream_reference istream_,
		size_t window_size_,
		size_t read_buffer_size_,
		size_t input_buffer_size_
	)
	:  
		m_istream(istream_),
		m_input_buffer(input_buffer_size_),
		m_buffer(read_buffer_size_)
	{
		// setting zalloc, zfree and opaque
		m_zip_stream.zalloc=(alloc_func)0;
		m_zip_stream.zfree=(free_func)0;

		m_zip_stream.next_in=NULL;
		m_zip_stream.avail_in=0;
		m_zip_stream.avail_out=0;
		m_zip_stream.next_out=NULL;


		m_err=inflateInit2(
			&m_zip_stream,
			static_cast<int>(window_size_)
		);
		
		setg( &(m_buffer[0])+4,     // beginning of putback area
		    &(m_buffer[0])+4,     // read position
	        &(m_buffer[0])+4);    // end position    
	};

    template<class Elem, class Tr>
    size_t basic_unzip_streambuf<Elem,Tr>::fill_input_buffer()
	{
		m_zip_stream.next_in=&(m_input_buffer[0]);
		m_istream.read( 
			(char_type*)(&(m_input_buffer[0])), 
			static_cast<std::streamsize>(m_input_buffer.size()/sizeof(char_type)) 
			); 
		return m_zip_stream.avail_in=m_istream.gcount()*sizeof(char_type);
	}


    template<class Elem, class Tr>
    basic_unzip_streambuf<Elem,Tr>::~basic_unzip_streambuf()
	{
		inflateEnd(&m_zip_stream);
	};

    template<class Elem, class Tr>
    typename basic_unzip_streambuf<Elem,Tr>::int_type 
		basic_unzip_streambuf<Elem,Tr>::underflow() 
	{ 
		if ( gptr() && ( gptr() < egptr()))
			return * reinterpret_cast<unsigned char *>( gptr());
     
       int n_putback = static_cast<int>(gptr() - eback());
       if ( n_putback > 4)
          n_putback = 4;
       memcpy( 
			&(m_buffer[0]) + (4 - n_putback), 
			gptr() - n_putback, 
			n_putback*sizeof(char_type)
			);
  
	   int num = unzip_from_stream( 
		   &(m_buffer[0])+4, 
		   static_cast<std::streamsize>((m_buffer.size()-4)*sizeof(char_type))
		   );
        if (num <= 0) // ERROR or EOF
           return EOF;
    
        // reset buffer pointers
        setg( &(m_buffer[0]) + (4 - n_putback),   // beginning of putback area
              &(m_buffer[0]) + 4,                 // read position
              &(m_buffer[0]) + 4 + num);          // end of buffer
    
         // return next character
         return* reinterpret_cast<unsigned char *>( gptr());    
     }


    template<class Elem, class Tr>
    std::streamsize basic_unzip_streambuf<Elem,Tr>::unzip_from_stream( 
		char_type* buffer_, 
		std::streamsize buffer_size_
		)
	{
		m_zip_stream.next_out=(byte_buffer_type)buffer_;
		m_zip_stream.avail_out=static_cast<uInt>(buffer_size_*sizeof(char_type));
		size_t count =m_zip_stream.avail_in;

		do
		{
			if (m_zip_stream.avail_in==0)
				count=fill_input_buffer();

			if (m_zip_stream.avail_in)
			{
				m_err = inflate( &m_zip_stream,  Z_SYNC_FLUSH );
			}
		} while (m_err==Z_OK && m_zip_stream.avail_out != 0 && count != 0);

		return buffer_size_ - m_zip_stream.avail_out/sizeof(char_type);
	}

	template<class Elem, class Tr>
	int basic_zip_istream<Elem,Tr>::check_header()
	{
	    int method; /* method byte */
	    int flags;  /* flags byte */
	    uInt len;
		int c;
		int err=0;
		z_stream& zip_stream = rdbuf()->get_zip_stream();

	    /* Check the gzip magic header */
		 for (len = 0; len < 2; len++) 
		 {
			c = (int)rdbuf()->get_istream().get();
			if (c != detail::gz_magic[len]) 
			{
			    if (len != 0) 
					rdbuf()->get_istream().unget();
			    if (c != EOF) 
			    {
					rdbuf()->get_istream().unget();
			    }
		    
			    err = zip_stream.avail_in != 0 ? Z_OK : Z_STREAM_END;
			    return err;
			}
		}
    
		method = (int)rdbuf()->get_istream().get();
		flags = (int)rdbuf()->get_istream().get();
		if (method != Z_DEFLATED || (flags & detail::gz_reserved) != 0) 
		{
			err = Z_DATA_ERROR;
			return err;
		}

	    /* Discard time, xflags and OS code: */
	    for (len = 0; len < 6; len++) 
			rdbuf()->get_istream().get();
	
	    if ((flags & detail::gz_extra_field) != 0) 
	    { 
			/* skip the extra field */
			len  =  (uInt)rdbuf()->get_istream().get();
			len += ((uInt)rdbuf()->get_istream().get())<<8;
			/* len is garbage if EOF but the loop below will quit anyway */
			while (len-- != 0 && rdbuf()->get_istream().get() != EOF) ;
	    }
	    if ((flags & detail::gz_orig_name) != 0) 
	    { 
			/* skip the original file name */
			while ((c = rdbuf()->get_istream().get()) != 0 && c != EOF) ;
		}
	    if ((flags & detail::gz_comment) != 0) 
	    {   
			/* skip the .gz file comment */
			while ((c = rdbuf()->get_istream().get()) != 0 && c != EOF) ;
		}
		if ((flags & detail::gz_head_crc) != 0) 
		{  /* skip the header crc */
			for (len = 0; len < 2; len++) 
				rdbuf()->get_istream().get();
		}
		err = rdbuf()->get_istream().eof() ? Z_DATA_ERROR : Z_OK;

		return err;
	}

	template<class Elem, class Tr>
	basic_zip_ostream<Elem,Tr>& basic_zip_ostream<Elem,Tr>::add_header()
	{
	    char_type buffer[256];
		int nwritten=sprintf(buffer,"%c%c%c%c%c%c%c%c%c%c", 
			detail::gz_magic[0], detail::gz_magic[1],
             Z_DEFLATED, 
             0 /*flags*/, 
             0,0,0,0 /*time*/, 
             0 /*xflags*/, 
             OS_CODE);
             
        rdbuf()->get_ostream().write(buffer,nwritten);
        
        return *this;
	}

}; // zlib_sream

#endif