#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define URLLEN 1000
#define HOSTNAMELEN 1000
#define WEBPAGELEN 1000
#define REQUESTLEN 1024
#define RESPONSELEN 100000
#define HYPERLINKLEN 1000
#define PORTNUM 80

int hostname_to_IP(char *, char *);

int main(void) {
    char *URL = calloc(URLLEN, sizeof(char));
    char *hostname = calloc(HOSTNAMELEN, sizeof(char));
    char *webpage = calloc(WEBPAGELEN, sizeof(char));
    char *request_message = calloc(REQUESTLEN, sizeof(char));
    char *response_message = calloc(RESPONSELEN, sizeof(char));
    char server_ip[100];
    char *hyperlink = calloc(HYPERLINKLEN, sizeof(char));

    char target[] = "a href=";
    char *target_ptr = &target[0];
    char *match_pos[105];

    int client_socket;
    struct sockaddr_in serverAddr;

    // Scan URL
    printf("Enter the hostname: ");
    scanf("%s", URL);

    // Split URL into (hostname + webpage)
    if (strncmp(URL, "http://", 7) == 0) {
        URL = &URL[7];
    }
    if (strncmp(URL, "https://", 8) == 0) {
        URL = &URL[8];
    }
    hostname = strtok(URL, "/");
    webpage = strtok(NULL, "");

    // Convert hostname to IP
    if (hostname_to_IP(hostname, server_ip) < 0) {
        return -1;
    }

    // Format request_message
    snprintf(request_message, REQUESTLEN,
             "GET /%s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n",
             (webpage == NULL) ? "" : webpage, hostname);

    // Create socket
    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORTNUM);
    serverAddr.sin_addr.s_addr = inet_addr(server_ip);
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    // Connect
    if (connect(client_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    // Send request
    printf("socket: Start send HTTP request\n");
    send(client_socket, request_message, REQUESTLEN, 0);

    // Receive response
    printf("socket: Start read the response\n");
    while (1) {
        char buffer[1024] = "";
        if (recv(client_socket, buffer, 1024, 0) <= 0) break;
        strncat(response_message, buffer, 1024);
    }

    // Read the response
    printf("socket: Finish read to buffer\n");
    printf("======== Hyperlinks ========\n");

    // Find all match positions
    int hyper_count = 0;
    response_message = strstr(response_message, target_ptr);
    while (response_message != NULL) {
        match_pos[hyper_count++] = response_message;
        response_message = strstr(response_message + strlen(target), target_ptr);
    }

    // Print the hyperlinks at match positions
    for (int i = 0; i < hyper_count; i++) {
        hyperlink = strtok(match_pos[i] + strlen(target), "\"");
        printf("%s\n", hyperlink);
    }

    return 0;
}

int hostname_to_IP(char *hostname, char *ip) {
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname");
        return -1;
    }

    addr_list = (struct in_addr **)he->h_addr_list;
    strcpy(ip, inet_ntoa(*addr_list[0]));

    return 0;
}

// can.cs.nthu.edu.tw/index.php
