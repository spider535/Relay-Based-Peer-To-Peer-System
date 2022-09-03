#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>

void error(const char *msg){
	perror(msg); //show the error
	exit(1); // 1 means true
}

void Display(){
	printf("-----------------------------------------------------------------------------------\n");
	printf("Server IP\tPort\toperation\tprotocol\tMore Info..\n");
	printf("-----------------------------------------------------------------------------------\n");
}

//comparison of two strings.
int equal(char a[],char buffer[]){
	for(int i=0;i<strlen(a);i++){
		if(a[i] != buffer[i]){
			return 0;
		}
	}
	return 1;
}

int startserver(char * port)//in order to start the server we only need port number
{
	int sockfd, newsockfd,portno;
     	socklen_t clilen;
     	char buffer[256];
     	struct sockaddr_in serv_addr, cli_addr;
     	int n;

     	sockfd = socket(AF_INET, SOCK_STREAM, 0);
     	if (sockfd < 0){ 
        	error("ERROR opening socket");
	}
     	bzero((char *) &serv_addr, sizeof(serv_addr));
     	portno = atoi(port); 
     	serv_addr.sin_family = AF_INET;
     	serv_addr.sin_addr.s_addr = INADDR_ANY;
     	serv_addr.sin_port = htons(portno);
     	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){ 
     		error("ERROR on binding");
	}
     	listen(sockfd,5);
	
	printf(".........\t");
	printf("%d\t",portno);
	printf("Listen   \t");
	printf("tcp     \t");
	printf("Server running on peer node, listening.....\n");
     	
	clilen = sizeof(cli_addr);
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     	if (newsockfd < 0){ 
     		error("ERROR on accept");
	}
     	close(sockfd);
     	bzero(buffer,256);
     	n = read(newsockfd,buffer,255);
     	if (n < 0){
		error("ERROR reading from socket");	
	} 
     	
     	//check if the message is a request for file
     	char a[]="REQUEST : FILE : ";
     	int i,flag=0;
	if(equal(a,buffer) == 1){
		flag = 1;
	}else{
		flag = 0;	
	}

     	if(flag){
		printf("peerClient\t");
		printf("%d\t",ntohs(cli_addr.sin_port));
		printf("REQUEST \t");
		printf("tcp     \t");
     		printf("Received request for the file : %s from the client\n",&buffer[strlen(a)]);
     		FILE * file=fopen(&buffer[strlen(a)],"r");
     		if(file==NULL){
			printf("\t\t\t\t\t\t\trequested file NOT Found\n");
			char response[]="File NOT FOUND";
     			n = write(newsockfd,response,strlen(response));
     			if (n < 0){
				error("ERROR writing to socket");			
			} 
		}else{
			printf("\t\t\t\t\t\t\tFound the requested file \n");
		 	char response[]="File FOUND    ";
     			n = write(newsockfd,response,strlen(response));
     			if (n < 0){
				error("ERROR writing to socket");			
			} 
	
     			//send the file
    			fseek(file, 0, SEEK_END);
    			long fsize = ftell(file);
    			fseek(file, 0, SEEK_SET);//send the pointer to beginning of the file

    			char *string = malloc(fsize + 1);
    			fread(string, fsize, 1, file);
    			fclose(file);
    			printf("\t\t\t\t\t\t\tfile has the following content:\n\t\t\t\t\t\t\t%s",string);
    
    			//send the content to client
			n = write(newsockfd,string,strlen(string));
			if (n < 0) {
      				error("ERROR writing to socket");
			}
		}
	}else{
		printf("received request is not of file name,closing connection......\n");
		close(newsockfd);
	} 
	return 0; 
}


int main(int argc, char *argv[]) {
	int sockfd, portno, n;
   	struct sockaddr_in serv_addr;
   	struct hostent *server; //hostent is a structur comes under #include <netdb.h>
   
   	char buffer[256];
   
   	if (argc < 3) {
        	fprintf(stderr,"usage %s hostname port\n", argv[0]);
      		exit(0);
   	}
	
   	portno = atoi(argv[2]);
   
   	/* Create a socket point */
   	sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   	if (sockfd < 0) {
      		error("ERROR opening socket");
   	}
	
   	server = gethostbyname(argv[1]);
   
   	if (server == NULL) {
      		fprintf(stderr,"ERROR, no such host\n");
      		exit(0);
   	}
   
   	bzero((char *) &serv_addr, sizeof(serv_addr));
   	serv_addr.sin_family = AF_INET;
   	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);/*bcopy is use to copy the bytes from server to serv_addr*/
   	serv_addr.sin_port = htons(portno); /*The htons function can be used to convert an IP port number in host byte order to the IP port number in 						      network byte order.*/
   
   	/* Now connect to the server */
   	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      		error("ERROR connecting");
   	}
   
   	/* Now ask for a message from the user, this message
      	   will be read by server
   	*/
	Display();
	printf("%s\t",argv[1]);
	printf("%d\t",portno);
	printf("connect  \t");
	printf("tcp     \t");
	printf("connection initiated to server\n");

   	char* req="REQUEST : node";
   
   	/* Send message to the server */
   	n = write(sockfd, req, strlen(req));
   
   	if (n < 0) {
      		error("ERROR writing to socket");
   	}
   
   	/* Now read The server response */
   	bzero(buffer,256);
   	n = read(sockfd, buffer, 255);
   
   	if (n < 0) {
      		error("ERROR reading from socket");
   	}
   
   	//start server if node accepted by relay
   	if(buffer[17]=='N'){
		printf("%s\t",argv[1]);
		printf("%d\t",portno);
		printf("RESPONSE\t");
		printf("tcp     \t");
		printf("Response from the server : NODE connected SUCESSFULLY \n");

	   	n = shutdown(sockfd,0);
	    	if (n < 0) {
			error("ERROR closing the connection");
		}
	   	//start the server
	   	startserver(&buffer[20]); //Now start the server with the port number stored at &buffer[20].
   	}else{
		printf("Node not accepted by the relay server\n");	
	} 
   
   	return 0;
}

