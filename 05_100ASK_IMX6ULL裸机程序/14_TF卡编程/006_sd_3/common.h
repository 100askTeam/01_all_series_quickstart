#ifndef   __COMMON_H__
#define   __COMMON_H__

#include "fsl_iomuxc.h"
#include "my_printf.h"

// define a macro that unconditionally swaps
#define SWAP_64(x) \
		((((x) & 0xff00000000000000ull) >> 56) \
		| (((x) & 0x00ff000000000000ull) >> 40) \
		| (((x) & 0x0000ff0000000000ull) >> 24) \
		| (((x) & 0x000000ff00000000ull) >> 8) \
		| (((x) & 0x00000000ff000000ull) << 8) \
		| (((x) & 0x0000000000ff0000ull) << 24) \
		| (((x) & 0x000000000000ff00ull) << 40) \
		| (((x) & 0x00000000000000ffull) << 56))
#define SWAP_32(x) \
	(((uint32_t)(x) << 24) | (((uint32_t)(x) & 0xff00) << 8) |(((uint32_t)(x) & 0x00ff0000) >> 8) | ((uint32_t)(x) >> 24))
#define SWAP_16(x) \
	((((uint16_t)(x) & 0xff) << 8) | ((uint16_t)(x) >> 8))


#ifndef NULL
#define NULL        0
#endif

#endif
