#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h> 
#include <sys/wait.h>
#include <pthread.h>
#define TIMEOUT 100
#define WND_SIZE 4
/*****************notice**********************
 * 
 * You can follow the comment inside the code.
 * This kind of comment is for basic part.
 * =============== 
 * Some hints...
 * ===============
 * 
 * This kind of comment is for bonus part.
 * ---------------
 * Some hints...
 * ---------------
 * 
 * 
 * 
 *********************************************/


//==============
// Packet header 
//==============
typedef struct header{
	unsigned int seq_num;
	unsigned int ack_num;
	unsigned char isLast;
}Header;

//==================
// Udp packet & data 
//==================
typedef struct udp_pkt{
	Header header;
	char data[1024];
}Udp_pkt;

//=============
// Declaration
//=============
int sockfd;
struct sockaddr_in info, client_info;
Udp_pkt snd_pkt,rcv_pkt;
socklen_t len;
pthread_t th1,th2;
int first_time_create_thread = 0;

time_t sentTime;

//---------------------------------------
// Declare for critical section in bonus. 
//---------------------------------------
/*******************notice*******************************
 * 
 * If you dont need the bonus point, ignore this comment.
 * Use it like following block.
 * 
 * pthread_mutex_lock( &mutex );
 * ...
 * critical section
 * ...
 * pthread_mutex_unlock( &mutex );
 * 
 *********************************************************/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


//------------------------------
// Bonus part for timeout_thread
//------------------------------
/*******************notice***************************
 * 
 * In bonus part, you should use following threads to
 * checking timeout and receive client ack.
 * 
 ***************************************************/

int done;
void* receive_thread(){
	//--------------------------------------
	// Checking timeout & Receive client ack
	//--------------------------------------
	while(1){
    	// pthread_mutex_lock(&mutex);
		if (recvfrom(sockfd, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&info, (socklen_t *)&len) >= 0){
			printf("Receive a packet ack_num = %d\n", rcv_pkt.header.ack_num);
			if (rcv_pkt.header.ack_num == snd_pkt.header.seq_num){
				done = 1;
				// pthread_mutex_unlock(&mutex);
				pthread_exit(NULL);
				return NULL;
			}
		}
		// pthread_mutex_unlock(&mutex);
	}
	
	// return NULL;
	//------------------------------------------
	// Keep the thread alive not to umcomment it
	//------------------------------------------
	// pthread_exit(NULL);
}

//------------------------------
// Bonus part for timeout_thread
//------------------------------
void* timeout_thread(){

	// clock_t sentTime;
	//sentTime = (clock()*1000)/CLOCKS_PER_SEC;

	while(1){
	    // pthread_mutex_lock(&mutex);

		if (done == 1) {
			// pthread_mutex_unlock(&mutex);
			pthread_exit(NULL);
			return NULL;
		}
		if ((clock()*1000)/CLOCKS_PER_SEC - sentTime >= TIMEOUT){
			sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0,(struct sockaddr *)&client_info, len);
			printf("Timout! Resend packet!\n");
			printf("Send a pack seq_num = %d\n", snd_pkt.header.seq_num);
			sentTime = (clock()*1000)/CLOCKS_PER_SEC;
		}
		// pthread_mutex_unlock(&mutex);
	}


	//------------------------------------------
	// Keep the thread alive not to umcomment it
	//------------------------------------------
	// pthread_exit(NULL);
}

//==================================
// You should complete this function
//==================================
// Send file function, it call receive_thread function at the first time.
int sendFile(FILE *fd){	
	int filesize=ftell(fd);
	//----------------------------------------------------------------
	// Bonus part for declare timeout threads if you need bonus point,
	// umcomment it and manage the thread by youself
	//----------------------------------------------------------------
	// At the first time, we need to create thread.
	if(!first_time_create_thread){
		first_time_create_thread=1;
		// pthread_create(&th1, NULL, receive_thread, NULL);
		// pthread_create(&th2, NULL, timeout_thread, NULL);
	}
	/*******************notice************************
	 * 
	 * In basic part, you should finish this function.
	 * You can try test_clock.c for clock() usage.
	 * checking timeout and receive client ack.
	 * 
	 ************************************************/
	int seq_number = 0;
	fseek(fd, 0, SEEK_SET);
	snd_pkt.header.isLast = 0;
	// clock_t sentTime;

	while(filesize > 0){
		int readSize = fread(snd_pkt.data, 1, 1024, fd);	
	//==========================
	// Send video data to client
	//==========================
		sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0,(struct sockaddr *)&client_info, len);
		printf("Send a pack seq_num = %d\n", snd_pkt.header.seq_num);
		sentTime = (clock()*1000)/CLOCKS_PER_SEC;
	//======================================
	// Checking timeout & Receive client ack
	//======================================	
		// while(1){
		// 	// if ((clock()*1000)/CLOCKS_PER_SEC - sentTime >= TIMEOUT){
		// 	// 	sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0,(struct sockaddr *)&client_info, len);
		// 	// 	printf("Timout! Resend packet!");
		// 	// 	printf("Send a pack seq_num = %d\n", snd_pkt.header.seq_num);
		// 	// 	sentTime = (clock()*1000)/CLOCKS_PER_SEC;
		// 	// }
		// 	// if (recvfrom(sockfd, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&info, (socklen_t *)&len) >= 0){
		// 	// 	printf("Receive a packet ack_num = %d\n", rcv_pkt.header.ack_num);
		// 	// 	if (rcv_pkt.header.ack_num == seq_number){
		// 	// 		break;
		// 	// 	}
		// 	// }
		// }
		done = 0;
		pthread_create(&th1, NULL, receive_thread, NULL);
		pthread_create(&th2, NULL, timeout_thread, NULL);
		pthread_join(th1, NULL);
		pthread_join(th2, NULL);

		// pthread_exit(th1);
		// pthread_exit(th2);
	//=============================================
	// Set is_last flag for the last part of packet
	//=============================================
		filesize -= readSize;
		seq_number ++;
		snd_pkt.header.seq_num = seq_number;
		if (filesize <= 1024){
			snd_pkt.header.isLast = 1;
		}
	}
	printf("send file successfully\n");
	fclose(fd);
	return 0;
}

int main(int argc, char *argv[]){
	//===========================
	// argv[1] is for server port
	//===========================
	sockfd = socket(AF_INET , SOCK_DGRAM , 0);

	if (sockfd == -1){
		printf("Fail to create a socket.");
	}
	//=======================
	// input server info
	// IP address = 127.0.0.1
	//=======================
	bzero(&info,sizeof(info));
	info.sin_family = AF_INET;
	int port=atoi(argv[1]);
	info.sin_addr.s_addr = INADDR_ANY ;
	info.sin_port = htons(port);
	//printf("server %s : %d\n", inet_ntoa(info.sin_addr), htons(info.sin_port));

	//================
	// Bind the socket
	//================	
	if(bind(sockfd, (struct sockaddr *)&info, sizeof(info)) == -1){
		perror("server_sockfd bind failed: ");
		return 0;
	}

	//====================================
	// Create send packet & receive packet
	//====================================
	memset(rcv_pkt.data, '\0', sizeof(rcv_pkt.data));

	//====================
	// Use for client info
	//====================
	bzero(&client_info,sizeof(client_info));
	client_info.sin_family = AF_INET;
	len = sizeof(client_info);

	printf("====Parameter====\n");
	printf("Server's IP is 127.0.0.1\n");
	printf("Server is listening on port %d\n",port);
	printf("==============\n");

	while(1){
		//=========================
		// Initialization parameter
		//=========================
		snd_pkt.header.seq_num = 0;
		snd_pkt.header.ack_num = 0;
		snd_pkt.header.isLast = 0;
		FILE *fd;
		
		printf("server waiting.... \n");
		char *str;
		while ((recvfrom(sockfd, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&client_info, (socklen_t *)&len)) != -1){
			//In client, we set is_last 1 to comfirm server get client's first message.
			if(rcv_pkt.header.isLast == 1)
				break;
		}  
		printf("process command.... \n");
		str = strtok(rcv_pkt.data, " ");
		
		//===============================================================
		// command "download filename": download the file from the server
		// and then check if filename is exist
		//===============================================================
		if(strcmp(str, "download") == 0) {
			str = strtok(NULL, " \n");
			printf("filename is %s\n",str);
			//===================
			// if file not exists 
			//===================
			if((fd = fopen(str, "rb")) == NULL) {    
				//=======================================
				// Send FILE_NOT_EXISTS msg to the client
				//=======================================
				printf("FILE_NOT_EXISTS\n");
                		strcpy(snd_pkt.data, "FILE_NOT_EXISTS");
				int numbytes;
				if ((numbytes = sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0,(struct sockaddr *)&client_info, len)) == -1) {
					printf("sendto error\n");
					return 0;
				}
				printf("server: sent %d bytes to %s\n", numbytes,inet_ntoa(client_info.sin_addr));
			} 
			//==================
			// else, file exists 
			//==================
			else {
				fseek(fd,0,SEEK_END);
                		printf("FILE_EXISTS\n");
				strcpy(snd_pkt.data, "FILE_EXISTS");
				
				
				//==================================
				// Send FILE_EXIST msg to the client
				//==================================
				int numbytes;
				if ((numbytes = sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0,(struct sockaddr *)&client_info, len)) == -1) {
					printf("sendto error\n");
					return 0;
				}
				printf("server: sent %d bytes to %s\n", numbytes,inet_ntoa(client_info.sin_addr));

				//==========================================================================
				// Sleep 1 seconds before transmitting data to make sure the client is ready
				//==========================================================================
				sleep(1);
				printf("trasmitting...\n");

				//=====================================
				// Start to send the file to the client
				//=====================================

				sendFile(fd);
				
			}
		}else {
			printf("Illegal request!\n");   
		}
	}
}
