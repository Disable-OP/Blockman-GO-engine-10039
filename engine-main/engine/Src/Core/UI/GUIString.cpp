#include "GUIString.h"

#include <iostream>

namespace LORD
{

// definition of 'no position' value
const GUIString::size_type GUIString::npos = (GUIString::size_type)(-1);


//////////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////////
GUIString::~GUIString(void)
{
	if (d_reserve > STR_QUICKBUFF_SIZE)
	{
		LordFree(d_buffer);
		d_buffer = NULL;
	}
	if (d_encodedbufflen > 0)
	{
		LordFree(d_encodedbuff);
		d_encodedbuff = NULL;
	}
}

bool GUIString::grow(size_type new_size)
{
    // check for too big
    if (max_size() <= new_size)
        std::length_error("Resulting CEGUI::GUIString would be too big");

    // increase, as we always null-terminate the buffer.
    ++new_size;

    if (new_size > d_reserve)
    {
        //utf16* temp = new utf16[new_size];

        //if (d_reserve > STR_QUICKBUFF_SIZE)
        //{
        //    memcpy(temp, d_buffer, (d_cplength + 1) * sizeof(utf16));
        //    delete[] d_buffer;
        //}
        //else
        //{
        //    memcpy(temp, d_quickbuff, (d_cplength + 1) * sizeof(utf16));
        //}

		utf16* temp = (utf16*)LordMalloc(new_size*sizeof(utf16));

		if (d_reserve > STR_QUICKBUFF_SIZE)
		{
			memcpy(temp, d_buffer, (d_cplength + 1) * sizeof(utf16));
			LordFree(d_buffer);
			d_buffer = NULL;
		}
		else
		{
			memcpy(temp, d_quickbuff, (d_cplength + 1) * sizeof(utf16));
		}

        d_buffer = temp;
        d_reserve = new_size;

        return true;
    }

    return false;
}

// perform re-allocation to remove wasted space.
void GUIString::trim(void)
{
    size_type min_size = d_cplength + 1;

    // only re-allocate when not using quick-buffer, and when size can be trimmed
    if ((d_reserve > STR_QUICKBUFF_SIZE) && (d_reserve > min_size))
    {
            // see if we can trim to quick-buffer
        if (min_size <= STR_QUICKBUFF_SIZE)
        {
            //memcpy(d_quickbuff, d_buffer, min_size * sizeof(utf16));
            //delete[] d_buffer;
			memcpy(d_quickbuff, d_buffer, min_size * sizeof(utf16));
			LordFree(d_buffer);
			d_buffer = NULL;
            d_reserve = STR_QUICKBUFF_SIZE;
        }
        // re-allocate buffer
        else
        {
            //utf16* temp = new utf16[min_size];
            //memcpy(temp, d_buffer, min_size * sizeof(utf16));
            //delete[] d_buffer;

			utf16* temp = (utf16*)LordMalloc(min_size*sizeof(utf16));
			memcpy(temp, d_buffer, min_size * sizeof(utf16));
			LordFree(d_buffer);
            d_buffer = temp;
            d_reserve = min_size;
        }

    }

}

// build an internal buffer with the string encoded as utf8 (remains valid until string is modified).
utf8* GUIString::build_utf8_buff(void) const
{
    size_type buffsize = encoded_size(ptr(), d_cplength) + 1;

    if (buffsize > d_encodedbufflen) {

        if (d_encodedbufflen > 0)
        {
            //delete[] d_encodedbuff;
			LordFree(d_encodedbuff);
			d_encodedbuff = NULL;
        }

        // d_encodedbuff = new utf8[buffsize];
		d_encodedbuff = (utf8*)LordMalloc(buffsize*sizeof(utf8));
        d_encodedbufflen = buffsize;
    }

    encode(ptr(), d_encodedbuff, buffsize, d_cplength);

    // always add a null at end
    d_encodedbuff[buffsize-1] = ((utf8)0);
    d_encodeddatlen = buffsize;

    return d_encodedbuff;
}

//////////////////////////////////////////////////////////////////////////
// Comparison operators
//////////////////////////////////////////////////////////////////////////
bool	operator==(const GUIString& str1, const GUIString& str2)
{
	return (str1.compare(str2) == 0);
}

bool	operator==(const GUIString& str, const std::string& std_str)
{
	return (str.compare(std_str) == 0);
}

bool	operator==(const std::string& std_str, const GUIString& str)
{
	return (str.compare(std_str) == 0);
}

bool	operator==(const GUIString& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) == 0);
}

bool	operator==(const utf8* utf8_str, const GUIString& str)
{
	return (str.compare(utf8_str) == 0);
}


bool	operator!=(const GUIString& str1, const GUIString& str2)
{
	return (str1.compare(str2) != 0);
}

bool	operator!=(const GUIString& str, const std::string& std_str)
{
	return (str.compare(std_str) != 0);
}

bool	operator!=(const std::string& std_str, const GUIString& str)
{
	return (str.compare(std_str) != 0);
}

bool	operator!=(const GUIString& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) != 0);
}

bool	operator!=(const utf8* utf8_str, const GUIString& str)
{
	return (str.compare(utf8_str) != 0);
}


bool	operator<(const GUIString& str1, const GUIString& str2)
{
	return (str1.compare(str2) < 0);
}

bool	operator<(const GUIString& str, const std::string& std_str)
{
	return (str.compare(std_str) < 0);
}

bool	operator<(const std::string& std_str, const GUIString& str)
{
	return (str.compare(std_str) >= 0);
}

bool	operator<(const GUIString& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) < 0);
}

bool	operator<(const utf8* utf8_str, const GUIString& str)
{
	return (str.compare(utf8_str) >= 0);
}


bool	operator>(const GUIString& str1, const GUIString& str2)
{
	return (str1.compare(str2) > 0);
}

bool	operator>(const GUIString& str, const std::string& std_str)
{
	return (str.compare(std_str) > 0);
}

bool	operator>(const std::string& std_str, const GUIString& str)
{
	return (str.compare(std_str) <= 0);
}

bool	operator>(const GUIString& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) > 0);
}

bool	operator>(const utf8* utf8_str, const GUIString& str)
{
	return (str.compare(utf8_str) <= 0);
}


bool	operator<=(const GUIString& str1, const GUIString& str2)
{
	return (str1.compare(str2) <= 0);
}

bool	operator<=(const GUIString& str, const std::string& std_str)
{
	return (str.compare(std_str) <= 0);
}

bool	operator<=(const std::string& std_str, const GUIString& str)
{
	return (str.compare(std_str) >= 0);
}

bool	operator<=(const GUIString& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) <= 0);
}

bool	operator<=(const utf8* utf8_str, const GUIString& str)
{
	return (str.compare(utf8_str) >= 0);
}


bool	operator>=(const GUIString& str1, const GUIString& str2)
{
	return (str1.compare(str2) >= 0);
}

bool	operator>=(const GUIString& str, const std::string& std_str)
{
	return (str.compare(std_str) >= 0);
}

bool	operator>=(const std::string& std_str, const GUIString& str)
{
	return (str.compare(std_str) <= 0);
}

bool	operator>=(const GUIString& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) >= 0);
}

bool	operator>=(const utf8* utf8_str, const GUIString& str)
{
	return (str.compare(utf8_str) <= 0);
}

//////////////////////////////////////////////////////////////////////////
// c-string operators
//////////////////////////////////////////////////////////////////////////
bool operator==(const GUIString& str, const char* c_str)
{
	return (str.compare(c_str) == 0);
}

bool operator==(const char* c_str, const GUIString& str)
{
	return (str.compare(c_str) == 0);
}

bool operator!=(const GUIString& str, const char* c_str)
{
	return (str.compare(c_str) != 0);
}

bool operator!=(const char* c_str, const GUIString& str)
{
	return (str.compare(c_str) != 0);
}

bool operator<(const GUIString& str, const char* c_str)
{
	return (str.compare(c_str) < 0);
}

bool operator<(const char* c_str, const GUIString& str)
{
	return (str.compare(c_str) >= 0);
}

bool operator>(const GUIString& str, const char* c_str)
{
	return (str.compare(c_str) > 0);
}

bool operator>(const char* c_str, const GUIString& str)
{
	return (str.compare(c_str) <= 0);
}

bool operator<=(const GUIString& str, const char* c_str)
{
	return (str.compare(c_str) <= 0);
}

bool operator<=(const char* c_str, const GUIString& str)
{
	return (str.compare(c_str) >= 0);
}

bool operator>=(const GUIString& str, const char* c_str)
{
	return (str.compare(c_str) >= 0);
}

bool operator>=(const char* c_str, const GUIString& str)
{
	return (str.compare(c_str) <= 0);
}

//////////////////////////////////////////////////////////////////////////
// Concatenation operator functions
//////////////////////////////////////////////////////////////////////////
GUIString	operator+(const GUIString& str1, const GUIString& str2)
{
	GUIString temp(str1);
	temp.append(str2);
	return temp;
}

GUIString	operator+(const GUIString& str, const std::string& std_str)
{
	GUIString temp(str);
	temp.append(std_str);
	return temp;
}

GUIString	operator+(const std::string& std_str, const GUIString& str)
{
	GUIString temp(std_str);
	temp.append(str);
	return temp;
}

GUIString	operator+(const GUIString& str, const utf8* utf8_str)
{
	GUIString temp(str);
	temp.append(utf8_str);
	return temp;
}

GUIString	operator+(const utf8* utf8_str, const GUIString& str)
{
	GUIString temp(utf8_str);
	temp.append(str);
	return temp;
}

GUIString	operator+(const GUIString& str, utf16 code_point)
{
	GUIString temp(str);
	temp.append(1, code_point);
	return temp;
}

GUIString	operator+(utf16 code_point, const GUIString& str)
{
	GUIString temp(1, code_point);
	temp.append(str);
	return temp;
}

GUIString operator+(const GUIString& str, const char* c_str)
{
	utf8* p = (utf8*)c_str;
	GUIString tmp(str);
	tmp.append(p);
	return tmp;
}

GUIString operator+(const char* c_str, const GUIString& str)
{
	utf8* p = (utf8*)c_str;
	GUIString tmp(p);
	tmp.append(str);
	return tmp;
}

//////////////////////////////////////////////////////////////////////////
// Output (stream) functions
//////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& s, const GUIString& str)
{
	return s << str.c_str();
}

//////////////////////////////////////////////////////////////////////////
// Modifying operations
//////////////////////////////////////////////////////////////////////////
// swap the contents of str1 and str2
void	swap(GUIString& str1, GUIString& str2)
{
	str1.swap(str2);
}


}
