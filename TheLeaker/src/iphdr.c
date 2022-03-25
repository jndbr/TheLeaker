#include "iphdr.h"

uint16_t ip_checksum(struct iphdr *buffer){
	int size = sizeof(struct iphdr);
	uint16_t *slice =(uint16_t *) buffer;
	uint32_t check = 0;
	while(size > 1){
		check += *slice++;
		size -= sizeof(uint16_t);
	}
	if(size > 0){
		check += *(char *)slice;
	}
	while((check >> 16) != 0){
		check = (uint32_t) ((check >> 16) + ((uint16_t)check));
	}
	return ((uint16_t)~check);

}
