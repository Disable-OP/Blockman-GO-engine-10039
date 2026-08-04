/********************************************************************
filename: 	Encoder.h
file path:	dev\engine\Src\Core\Util\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ENCODER_H__
#define __LORD_ENCODER_H__

#define _ENCODING_LOWER        1
#define _ENCODING_UPPER        2
#define _ENCODING_HALF         4
#define _ENCODING_SIMPLIFIED   8

namespace LORD
{
	LORD_CORE_API void _EncoderInit();

	LORD_CORE_API void _NormalizeUTF8(char* text, unsigned options);
	// LORD_CORE_API void _NormalizeGBK(char* text, unsigned options);

	LORD_CORE_API void _GBK_TO_UTF8(const char* from, unsigned int from_len, char** to, unsigned int* to_len);
	LORD_CORE_API void _UTF8_TO_GBK(const char* from, unsigned int from_len, char** to, unsigned int* to_len);
}


#endif