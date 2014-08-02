#ifndef __LUACL_ENDIANNESS_HPP
#define __LUACL_ENDIANNESS_HPP

#include <stdint.h>

namespace util{
	enum luacl_byte_order_enumeration {
		LUACL_UNTESTED_ENDIAN,
		LUACL_LITTLE_ENDIAN,
		LUACL_BIG_ENDIAN,
		LUACL_MIXED_ENDIAN,
	};
	
	inline luacl_byte_order_enumeration luacl_host_byte_order(void) {

		static luacl_byte_order_enumeration byte_order = LUACL_UNTESTED_ENDIAN;
		if (byte_order != LUACL_UNTESTED_ENDIAN) {
			return byte_order;
		}

		char dest[sizeof(uint32_t)];
		uint32_t src = UINT32_C( 0x01020304 );
		/*
			According to ISO C, to reinterpret bits as another data type,
			the only strict correct method is to use `memcpy`.
		*/
		memcpy(dest, &src, sizeof(uint32_t));
		
		bool asc = true;
		bool desc = true;
		for (size_t i = 1; i < sizeof(uint32_t); i++) {
			asc = asc && dest[i - 1] < dest[i];
			desc = desc && dest[i - 1] > dest[i];
		}
		
		if (!asc && desc) {
			return byte_order = LUACL_LITTLE_ENDIAN;
		}
		else if (asc && !desc) {
			return byte_order = LUACL_BIG_ENDIAN;
		}
		else {
			return byte_order = LUACL_MIXED_ENDIAN;
		}
	}

	template <typename T>
	void luacl_byte_order_reverse(T * buffer, size_t size) {
        char * src = reinterpret_cast<char *>(buffer + 1);
		char wordBuffer[sizeof(T)];

		while (size--) {
			for (size_t i = 0; i < sizeof(T); i++) {
				memcpy(wordBuffer + i, --src, 1);
			}
			memcpy(buffer, wordBuffer, sizeof(T));

			src += sizeof(T) * 2;
			buffer++;
		}
	}
}

#endif /* __LUACL_ENDIANNESS_HPP */
