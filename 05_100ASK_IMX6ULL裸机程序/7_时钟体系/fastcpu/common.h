#ifndef COMMON_H
#define COMMON_H

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long long  u64;

#define read8(a)            (*(volatile unsigned char *)(a))
#define read16(a)           (*(volatile unsigned short *)(a))
#define read32(a)           (*(volatile unsigned int *)(a))
#define read64(a)           (*(volatile unsigned long long *)(a))

#define write8(v,a)         (*(volatile unsigned char *)(a) = (v))
#define write16(v,a)        (*(volatile unsigned short *)(a) = (v))
#define write32(v,a)        (*(volatile unsigned int *)(a) = (v))
#define write64(v,a)        (*(volatile unsigned long long *)(a) = (v))

static inline void set_bit(volatile u32 *reg_addr, u32 bit)
{
    u32 v = read32(reg_addr);
    v |= 1u << bit;
    write32(v, reg_addr);
}

static inline void clr_bit(volatile u32 *reg_addr, u32 bit)
{
    u32 v = read32(reg_addr);
    v &= ~(1u << bit);
    write32(v, reg_addr);
}

#endif	/* COMMON_H */
