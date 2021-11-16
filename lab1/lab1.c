#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

#define HOSTNAMELEN 105

int hostname_to_IP(char *, char *);

int main(void) {
    char *hostname = calloc(HOSTNAMELEN, sizeof(char));

    printf("Enter the hostname: ");
    scanf("%s", hostname);
	
	char ip[100];

	hostname_to_IP(hostname, ip);
	printf("%s resolved to %s\n", hostname, ip);

	return 0;

}

int hostname_to_IP(char *hostname, char *ip){
	struct hostent *he;
	struct in_addr **addr_list;
	
	if ((he = gethostbyname(hostname)) == NULL){
		herror("gethostbyname");
		return 1;
	}

	addr_list = (struct in_addr ** ) he->h_addr_list;

	for (int i = 0; addr_list[i] != NULL; i++){
		strcpy(ip, inet_ntoa(*addr_list[i]));
		return 0;
	}

	return 1;
}
