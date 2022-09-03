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


int connectpeer(char * address,int portno,char * filename) {
	int sockfd, n;
   	struct sockaddr_in serv_addr;
   	struct hostent *server;
   	struct in_addr ipv4addr;
   	char buffer[256];
        
     	sockfd = socket(AF_INET, SOCK_STREAM, 0);
     	if (sockfd < 0){
		error("ERROR opening socket");
	} 
   	inet_pton(AF_INET, address, &ipv4addr);
   	server = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
   
   	if (server == NULL) {
      		fprintf(stderr,"ERROR, no such host\n");
      		exit(0);
   	}
   
   	bzero((char *) &serv_addr, sizeof(serv_addr));
   	serv_addr.sin_family = AF_INET;
   	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   	serv_addr.sin_port = htons(portno);
   
   
   
   	/* Now connect to the server */
   	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      		error("ERROR connecting");
   	}
   
   	/* Now ask for a message from the user, this message
      	   will be read by server
   	*/
	
   	printf("\t\t\t\t\t\t\tConnection to the Peer SUCCESSFUL.\n");
   	char req[50];
   	char* buff="REQUEST : FILE :";
	sprintf(req,"%s %s",buff, filename);
   	printf("%s\t",address);
	printf("%d\t",portno);
	printf("REQUEST \t");
	printf("tcp     \t");
	printf("Request to the peerNode : %s\n", req);
   	/* Send message to the server */
   	n = write(sockfd, req, strlen(req));
   	if (n < 0) {
      		error("ERROR writing to socket");
   	}
        
   	/* Now read server response */
   	bzero(buffer,256);
   	n = read(sockfd, buffer, 14);
   	if (n < 0) {
      		error("ERROR reading from socket");
   	}
	printf("%s\t",address);
	printf("%d\t",portno);
	printf("RESPONSE\t");
	printf("tcp     \t");
	printf("Response from the peerNode : %s\n", buffer);
   	if(strcmp(buffer,"File NOT FOUND")==0){
		//close the connection gracefully since file not found
	   	printf("\t\t\t\t\t\t\tClosing the connection gracefully since file NOT FOUND on this node...\n");
	   	n=shutdown(sockfd,0);
	   	if (n < 0) {
			error("ERROR closing the connection");
		}
	}else  if(strcmp(buffer,"File FOUND    ")==0) {
		n = read(sockfd, buffer, 255);//read the file content the peer is sending
		if (n < 0) {
			error("ERROR reading from socket");
		}
		printf("\t\t\t\t\t\t\tFile has the following content - \n\t\t\t\t\t\t\t%s",buffer);
		printf("\t\t\t\t\t\t\tgracefully closing the connection with the peer....\n");
		n=shutdown(sockfd,0);
		if (n < 0) {
			error("ERROR closing the connection");
		}
		
		//save the file on the client too
		FILE * save=fopen("sample1.txt","w");
		fprintf(save,"%s",buffer);
		fclose(save);
		
		return 0;
	}else{
		printf("received unknown reply from the node\n");	
	} 
	return -1;
}


int getFile(int sockfd, char IP[], int portno){
	
	//request for active peer information
	char* req="REQUEST : peer info",buffer[256];
	int n;
   
   	/* Send message to the server */
   	n = write(sockfd, req, strlen(req));
   	
   	if (n < 0) {
      		error("ERROR writing to socket");
   	}
   	
   	/* Now read server response */
   	bzero(buffer,256);
   	n = read(sockfd, buffer, 255);
   
   	if (n < 0) {
      		error("ERROR reading from socket");
   	}
   	
	printf("%s\t",IP);
	printf("%d\t",portno);
	printf("RESPONSE\t");
	printf("tcp     \t");
	printf("Response from the server : \n");
	printf("Server has the following info:\n");
	printf("%s",buffer);
   	printf("gracefully closing the connection with the relay server....\n");
   	n = shutdown(sockfd,0);
	if (n < 0) {
		error("ERROR closing the connection");
	}
	//store the info in a file
	FILE * peers = fopen("peer.txt","w");
	fprintf(peers,"%s",buffer);//reading the peer info from the buffer that is comming from the relay server and storing it into peer.txt file
	fclose(peers);
      
   	char  file[50];
   	printf("Enter the File name : ");
   	scanf("%s",file);
   	//process the response one peer at a time and try to fetch the file
   	char peerName[INET_ADDRSTRLEN];
   	int port,flag=0;
   	peers=fopen("peer.txt","r");
   	while(fscanf(peers,"%s %d",peerName,&port)!=EOF){
		printf("%s\t",peerName);
		printf("%d\t",port);
		printf("connect  \t");
		printf("tcp     \t");
		printf("Connecting to the peerNode\n");
	   	n = connectpeer(peerName,port,file);
	   	if(n<0){
			continue;
		}else {
			flag=1;
			break;
		}//successfult found the file on this node
   	}
   	fclose(peers);
   	if(!flag){
		printf("File not found on any node!\n");	
	} 
	return 0;
}

int main(int argc, char *argv[]) {
	int sockfd, portno, n;
   	struct sockaddr_in serv_addr;
   	struct hostent *server;
   
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
   	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   	serv_addr.sin_port = htons(portno);
   
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
	printf("connection initiated to RELAY server\n");
   	
   	char* req="REQUEST : client";
   
   	/* Send message to the server */
   	n = write(sockfd, req, strlen(req));
   
  	if (n < 0) {
      		error("ERROR writing to socket");
   	}
   
   	/* Now read server response */
   	bzero(buffer,256);
   	n = read(sockfd, buffer, 255);
   
   	if (n < 0) {
      		error("ERROR reading from socket");
      	}
   
   	//start server if node accepted by relay
   	if(buffer[19]=='C'){
		printf("%s\t",argv[1]);
		printf("%d\t",portno);
		printf("RESPONSE\t");
		printf("tcp     \t");
		printf("Response from the server : CLIENT connected SUCESSFULLY \n");
		
		printf("%s\t",argv[1]);
		printf("%d\t",portno);
		printf("REQUEST \t");
		printf("tcp     \t");
		printf("Request to the server : peer Info \n");
	   	n = getFile(sockfd, argv[1], portno);
   		if (n < 0) {
      			error("ERROR getting the requested file from the peers");
   		}
	}else{
		printf("Node not accepted by the relay server, try again..\n");
	}
   	return 0;
}





