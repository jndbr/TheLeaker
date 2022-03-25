#ifndef IPHDR_H
#define IPHDR_H
#include <inttypes.h>
#include <asm/byteorder.h>
typedef struct iphdr{
#if defined(__BIG_ENDIAN_BITFIELD)
	uint8_t version : 4;  //
	uint8_t ihl : 4;      // Header Length
#elif defined(__LITTLE_ENDIAN_BITFIELD)
	uint8_t ihl : 4;      //
	uint8_t version : 4;      //Header Leangth
#endif
	uint8_t tos;	      // Type of Service
	uint16_t t_len;       // Total Length
	uint16_t id;          //
	uint16_t ffo;         // Flags and Fragment Offset
	uint8_t ttl;          // Time to Live
	uint8_t protocol;     //
	uint16_t checksum;    //
	uint32_t src_addr;    // Source Address
	uint32_t dst_addr;    // Destination Address
} iphdr;

/**
 * Function Name: ip_checksum
 * Description: 
 * @param struct iphdr * - A pointer to an IP Packet
 * @return uint16_t - The checksum of IP Packet
 * */
uint16_t ip_checksum(struct iphdr *);
#endif
