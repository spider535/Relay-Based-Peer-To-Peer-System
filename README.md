# Relay-based-Peer-to-Peer-System
This is a Relay based Peer-to-Peer System using Client-Server socket programming.

In this System Peer_Client, Relay_Server and Peer_Nodes communicate with each other based on TCP sockets

This System works in three phase shown below :
<img width="791" alt="Screenshot 2021-09-13 134740" src="https://user-images.githubusercontent.com/77717476/133048625-4e3311c5-3291-4530-b9f8-623e18bf4806.png">


* **Initially**, the Peer_Nodes (peer 1/2/3 as shown in Figure 1) will connect to the Relay_Server using the TCP port 
already known to them. After successful connection, all the Peer_Nodes provide their information (IP address 
and PORT) to the Relay_Server and close the connections (as shown in Figure 1). The Relay_Server actively 
maintains all the received information with it. Now the Peer_Nodes will act as servers and wait to accept 
connection from Peer_Clients (refer phase three).
* **In second phase**, the Peer_Client will connect to the Relay_Server using the server’s TCP port already known to 
it. After successful connection; it will request the Relay_Server for active Peer_Nodes information (as shown in 
Figure 2). The Relay_Server will response to the Peer_Client with the active Peer_Nodes information currently 
having with it. On receiving the response message from the Relay_Server, the Peer_Client closes the connection 
gracefully.
* **In third phase**, a set of files (say, *.txt) are distributed evenly among the three Peer_Nodes. The Peer_Client will 
take “file_Name” as an input from the user. Then it connects to the Peer_Nodes one at a time using the response 
information. After successful connection, the Peer_Client tries to fetches the file from the Peer_Node. If the file is 
present with the Peer_Node, it will provide the file content to the Peer_Client and the Peer_Client will print the 
file content in its terminal. If not, Peer_Client will connect the next Peer_Node and performs the above action. 
This will continue till the Peer_Client gets the file content or all the entries in the Relay_Server Response are 
exhausted.

**Instruction to run this system**

* **Open 5 terminals**
   * terminal 1 is for Server
	* terminal 2 is for peer1
	* terminal 3 is for peer2
	* terminal 4 is for peer3
	* terminal 5 is for Client
  
* **Compile the three codes in each folder Server, peer1, peer2, peer3 and Client**
  * gcc server.c -o server  
  * gcc peer_node.c -o peerNode  
  * gcc peer_client.c -o peerClient  

* **Run** 

      /server <serverport>
      
      /node <serveraddress> <serverport>  
     
      /node <serveraddress> <serverport>  
     
      /node <serveraddress> <serverport>  
	 
      /client <serveraddress> <serverport> 
  
	server address is localhost here
  
* The requested file should be with a peernode because The client will ask for a file name 
   and return the file if it is found with any node.

**Example :** 

1) Run server : 

       Compile : gcc server.c -o sr
                
       Run     : ./sr 8888

2) Run peer_node 1 : 
	     
       Compile : gcc peer_node.c -o pn1
	     
       Run     : ./pn1 127.0.0.1 8888
       
3) Run peer_node 2 :
 
	   Compile : gcc peer_node.c -o pn2
	     
       Run     : ./pn2 127.0.0.1 8888
       
4) Run peer_node 3 :
 
	   Compile : gcc peer_node.c -o pn3
	     
       Run     : ./pn3 127.0.0.1 8888

5) Run client :
 
	   Compile : gcc peer_client.c -o cl
	     
       Run     : ./cl 127.0.0.1 8888
