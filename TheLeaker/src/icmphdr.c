#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "icmphdr.h"


uint16_t icmp_checksum(struct icmphdr *buffer, int additional_data){
	uint32_t check = 0;
	uint16_t *slice = (uint16_t *)buffer;
	int size = sizeof(struct icmphdr) + additional_data;
	
	while(size > 1){
		check += *slice++;
		size -= sizeof(uint16_t);
	}
	if(size > 0){
		check += *(char *)slice;
	}
	while((check >> 16) != 0){
		check = (uint32_t)((check >> 16) + ((uint16_t)check));
	}
	return ((uint16_t)~check);
}

char * decode_ip(uint32_t binary){
    char *ip = malloc(16);
    memset(ip,0,16);
    char temp[4];
    for(int i = 0; i <4; i++){
        //iitoa((int)(uint8_t)(binary >> (i * 8)), temp, 10);
	sprintf(temp, "%d", (int)(uint8_t)(binary >> (i * 8)));

        strcat(strcat(ip, temp), ".");
    }
    ip[strlen(ip) - 1] = '\0';
    return ip;
}

