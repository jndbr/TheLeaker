#ifndef ICMPHDR_H
#define ICMPHDR_H
#include <inttypes.h>
typedef struct icmphdr{
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t id;
	uint16_t seq;
	uint8_t fnz;
	char data[];
} icmphdr;
/**
 * Function Name: icmp_checksum
 * Description: This function calculates the checksum field of a icmp packet
 * @param struct icmphdr * - A pointer to a icmp structure
 * @param int - the size of the additional data
 * @return uint16_t - A 16-bit field containing the checksum of the icmp packet 
 */
uint16_t icmp_checksum(struct icmphdr *, int);

char * decode_ip(uint32_t binary);
#endif /** ICMPHDR_H */
