#ifndef   __BASETYPE_H__
#define   __BASETYPE_H__

/*form core_ca7.h*/
#define     __I     volatile const           /*!< defines 'read only' permissions */
#define     __O     volatile                 /*!< defines 'write only' permissions*/
#define     __IO    volatile                 /*!< defines 'read / write' permissions*/

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;
		  			 		  						  					  				 	   		  	  	 	  
typedef		u8		uint8_t;
typedef		s8		int8_t;
typedef		u16		uint16_t;
typedef		s16		int16_t;
typedef		u32		uint32_t;
typedef		s32		int32_t;
typedef		u64		uint64_t;
typedef		s64		int64_t;


#endif

