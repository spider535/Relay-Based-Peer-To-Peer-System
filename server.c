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

FILE * output;
int count = 0;

//peerNode
int storeNodeInfo(int port, int sock, char clntName[]){
	int n;
	
	output = fopen("node_info.txt","a+"); // append mode 
	fprintf(output,"%s %d\n",clntName,port);  
	fclose(output);
			   
	char* resp="RESPONSE : Node: N";
	char comma = ',';
	char buffer[50];
	sprintf(buffer,"%s%c %d", resp, comma , port);/*it returns the total number of characters written excluding null-character 					     			appended in the buffer*/
	printf("Server    \t");
	printf("%d\t",port);
	printf("RESPONSE\t");
	printf("tcp     \t");
	printf("Sending %s\n",resp);
	
	n = write(sock,buffer,strlen(buffer));
	if (n < 0) {
      		error("ERROR writing to socket");
		return 0;
   	}
	return 1;
}


//peerClient
void clientHandler(int sock, char buffer[], int port){
	int n;
	char* resp="RESPONSE : client: C";	
	printf("Server    \t");
	printf("%d\t",port);
	printf("RESPONSE\t");
	printf("tcp     \t");
	printf("Sending %s\n",resp);
	n = write(sock,resp,strlen(resp));	
	
	n = read(sock,buffer,255);
	if (n < 0) {
      		error("ERROR reading from socket");
   	}
	printf("peerClient\t");
	printf("%d\t",port);
	printf("REQUEST \t");
	printf("tcp     \t");
	printf("Request from the client - %s\n",buffer);
   	if(strcmp(buffer,"REQUEST : peer info")==0){
		//read the file that stored the peer info
		FILE *f = fopen("node_info.txt", "rb"); //open the file in binary format.

		/*SEEK_END : It denotes end of the file.
		  SEEK_SET : It denotes starting of the file.
		  SEEK_CUR : It denotes file pointer’s current position.
		*/
		fseek(f, 0, SEEK_END); //move the file pointer to the end
    		long fsize = ftell(f); //tell the file pointer position w.r.t. starting of the file (i.e. size of the file)
    		fseek(f, 0, SEEK_SET); //send the pointer to beginning of the file
		//above three lines is use to find the size of the file.					

						
    		char *string = malloc(fsize + 1);
    		fread(string, fsize, 1, f);
  		fclose(f);
		
    		string[fsize] = 0;
    		printf("Server has the following info:\n%s",string);
    		
    		//send the info to client
		n = write(sock,string,strlen(string)); //writing to the client
		if (n < 0) {
      			error("ERROR writing to socket");
		}
	}
}

void Display(){
	printf("-----------------------------------------------------------------------------------\n");
	printf("CLient Type\tPort\toperation\tprotocol\tMore Info..\n");
	printf("-----------------------------------------------------------------------------------\n");
}

int main( int argc, char *argv[] ) {

	if (argc < 2) {
      		fprintf(stderr,"usage %s port\n", argv[0]);
      		exit(0);
   	}
	
	output = fopen("node_info.txt","w");
	fclose(output);
	

   	int sockfd, newsockfd, portno, clilen;
   	char buffer[256];

   	struct sockaddr_in serv_addr, cli_addr;
   	int n, pid;  
   
   	/*socket creation*/
   	sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   	if (sockfd < 0) {
     		error("ERROR opening socket");
   	}
   
   	/* Initialize socket structure */
   	bzero((char *) &serv_addr, sizeof(serv_addr));//use to clear the data on server
   	portno = atoi(argv[1]);//conver the string into integer
   
   	serv_addr.sin_family = AF_INET; 
   	serv_addr.sin_addr.s_addr = INADDR_ANY;
   	serv_addr.sin_port = htons(portno); // host to network short 
   
   	/* Now bind the host address using bind() call to server socket*/
   	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      		error("ERROR on binding");
   	}
   
   	//Now start listening for the clients
   
   	listen(sockfd,5); //5 is the maximum limit of the client that can connect at a time to the server.
   	clilen = sizeof(cli_addr);
  	printf("Server started, now listening....to port number %d\n", portno);
   	Display();
	//her we are connecting client ot the server.
   	while (1) {
      		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); 
		count++;
      		if (newsockfd < 0) {
         		error("ERROR on accept");
      		}
            
      		/* Create child process */
      		pid = fork();
		
      		if (pid < 0) {
        	 	error("ERROR on fork");
      		}
      	
      		if (pid == 0) {
        	 	/* This is the client process */
        	 	close(sockfd);
    	     
   			int n, flag=0, port=ntohs(cli_addr.sin_port)+100, sock=newsockfd;
   			char buffer[256];
   			bzero(buffer,256);
   			n = read(sock,buffer,255); //reads the REQUEST message into the buffer comming from client.
   			if (n < 0) {
      				error("ERROR reading from socket");
   			}
			//Now checking whether request is comming from the peer_client or peer_node.
   			if(strcmp(buffer,"REQUEST : node")==0){
      				flag=1;
			}else if(strcmp(buffer,"REQUEST : client")==0){
      				flag=2;
			}
	
   	
   			if(flag==1){
				printf("peerNode%d\t",count);
				printf("%d\t",port);
				printf("CONNECT  \t");
				printf("tcp     \t");
				printf("Received Message - %s\n",buffer);
				
		 		/* Store the IP address and port number of the peerNode*/
				char clntName[INET_ADDRSTRLEN];
				//inet_ntop is use to convert network address into character string.
				//inet_ntop(AF_INET,&cli_addr.sin_addr.s_addr,clntName,sizeof(clntName)) equivalent to address in character string.
				if(inet_ntop(AF_INET,&cli_addr.sin_addr.s_addr,clntName,sizeof(clntName))!=NULL){ 
					if(storeNodeInfo(port, sock, clntName) == 1){
						//printf(" A peer is connected\n");					
					}					
				} else {
					printf("Unable to get address\n"); 
				}	
   			}else if(flag==2){
				//serving clients.
				printf("peerClient\t");
				printf("%d\t",port);
				printf("CONNECT  \t");
				printf("tcp     \t");
				printf("Received Message - %s\n",buffer);
				clientHandler(sock, buffer, port);
   			}else {
				printf("ERROR : Unknown REQEST message, no action taken\n");
			}        	 
        		exit(0);
      		}else {
        	 	close(newsockfd);
      		}	
   	} /* end of while */
}	


