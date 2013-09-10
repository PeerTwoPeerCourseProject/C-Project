/*Group 10 T-106.3101 Intermediate Course in Programming T2 Spring 2013 Project
* Tuukka Saarikoski ******
* Simo Haakana ******
* Patrick Patoila ******
*/

#include "dhtpackettypes.h"
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

#define MAX_CONNECTIONS 5
#define DHT_SERVER_SHAKE 0x413f
#define DHT_CLIENT_SHAKE 0x4121
#define IPADDRESS "127.0.0.1" // LOCAL HOST


// TYPEDEF STRUCT FILE, holds key/data pair.

typedef struct _file {
    unsigned char* key;
    unsigned char* data;
    uint16_t size;
    int removed;
}file;

typedef struct _file *File;



static unsigned char* hash;		// own hash
static unsigned char* address;	// own address
static unsigned char* left;		// left neighbour hash
static unsigned char* right;		// right neighbour hash

static int no_of_files;			// number of files stored in a node

static int* listensock; 			// listens own listenport
static int* connectsock;			// connection to server
static int* javasock;				// works with java
static unsigned char* target;
static unsigned char* sender;
static unsigned char* request_type;
static unsigned char* payload;

static uint16_t jtype;
static unsigned char* jkey;
static uint16_t* jlength;
static unsigned char* jlength2;
static unsigned char* jvalue;
static unsigned char nullhash[20];
static unsigned char max[20];



static uint16_t* length;
static uint16_t type;
static int registeracks = 0;


static file *filearray;				// stores file structs
static int arraysize;

// DOUBLING ARRAY SIZE

file *dbl_array(file *filearray, size_t newsize){

	file *newarray;

	newarray = realloc(filearray, newsize);

	return newarray;
}


void die(char *reason){

    fprintf(stderr, "Fatal error: %s\n", reason);
    exit(1);
}


int create_listen_socket(int listenport){

    int fd;
    int t;

    struct sockaddr_in a;

    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_family = AF_INET;
    a.sin_port = htons(listenport);

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
    	die(strerror(errno)); }

    t = bind(fd, (struct sockaddr *)(&a), sizeof(struct sockaddr_in));
    if (t == -1) {
    	die(strerror(errno)); }

    t = listen(fd, MAX_CONNECTIONS);
    if (t == -1) {
    	die(strerror(errno)); }

    return fd;
}

//Sends packets to java

void send_java_packet(uint16_t type){
	unsigned char typej[2];
	typej[0] = type >> 8;
	typej[1] = type & 0xff;
	send(*javasock, &typej, 2, 0);

	if (type == 3){
		unsigned char length3[2];
	    length3[0] = *length >> 8;
	    length3[1] = *length & 0xff;
	    send(*javasock, &length3, 2, 0);

		send(*javasock, payload, *length, 0);
	}
}

//Sends packets [target, sender, type, length of payload, (payload)]

void send_packet(int sock, uint16_t type, unsigned char* targetHash, unsigned char* senderHash, uint16_t length, unsigned char* payload){

    unsigned char requestType[4];
    requestType[0] = type >> 8;
    requestType[1] = type & 0xff;
    requestType[2] = length >> 8;
    requestType[3] = length & 0xff;

    unsigned char* pkt;
    pkt = malloc((44 + length));
    memcpy(pkt, targetHash, 20);
    memcpy(pkt + 20, senderHash, 20);
    memcpy(pkt + 40, &requestType, 4);

    puts("Sent packet:");

    printf("Target: ");
    for (int i = 0; i < 20; i++){
    	printf("%x", targetHash[i]);
    }

    printf("\nSender: ");
    for (int i = 0; i < 20; i++){
    	printf("%x", senderHash[i]);
    }

    printf("\nRequestType: ");
    for (int i = 0; i < 4; i++){
    	printf("%x", requestType[i]);
    }

    printf("\n");

    if (length != 0){
    	memcpy(pkt + 44, payload, length);
        printf("Payload: ");
        for (int i = 0; i < length; i++){
        	printf("%x", payload[i]);
        }

        printf("\n");
    }

    printf("\n");

    send(sock, pkt, 44 + length, 0);

    free(pkt);
}

// Receives packets from java [type, key(hash), (key(value))]

void receive_javapacket(int sock){

	recv(sock, &jtype, 1, 0);

	recv(sock, jkey, 20, 0);

	puts("Received JAVA packet:");

	printf("\nJAVA type: %d", jtype);


	printf("\nJAVA Key: ");
	for (int i = 0; i < 20; i++){
	    printf("%x", jkey[i]);
	}

	printf("\n");

	if (jtype == 1) {
		recv(sock, jlength2, 2, 0);

		*jlength = jlength2[0] * 256 + jlength2[1];

		printf("\nJ LENGTH: %d", *jlength);



		if (jlength != 0){
			free(jvalue);
			jvalue = malloc(*jlength);
			recv(sock, jvalue, *jlength, 0);

			printf("JAVA Value: ");
			for (int i = 0; i < *jlength; i++){
				printf("%x", jvalue[i]);
	    	}

			printf("\n");
		}
	}
	printf("\n");

}

// Receives packets [target, sender, type, length of payload, (payload)]

void receive_packet(int sock){

    recv(sock, target, 20, 0);

    recv(sock, sender, 20, 0);

    recv(sock, request_type, 4, 0);

    puts("Received packet:");

    printf("Target: ");
    for (int i = 0; i < 20; i++){
    	printf("%x", target[i]);
    }

    printf("\nSender: ");
    for (int i = 0; i < 20; i++){
    	printf("%x", sender[i]);
    }

    printf("\nRequestType: ");
    for (int i = 0; i < 4; i++){
    	printf("%x", request_type[i]);
    }

    printf("\n");

    *length = request_type[2] * 256 + request_type[3];
    type = request_type[0] * 256 + request_type[1];

    if (*length != 0){
        free(payload);
    	payload = malloc(*length);
        recv(sock, payload, *length, 0);

        printf("Payload: ");
        for (int i = 0; i < *length; i++){
        	printf("%x", payload[i]);
        }

        printf("\n");
    }
    printf("\n");
}

// Connects to other nodes

int connect_to_node(unsigned char* TCPaddress){

    uint16_t port = TCPaddress[0] * 256 + TCPaddress[1];

    unsigned char* ipaddress;
    ipaddress = malloc(9);
    ipaddress[9] = '\0';

    for (int q = 2; q < 11; q++){
    	ipaddress[q-2] = TCPaddress[q];
    }

	int sock;
	struct sockaddr_in node;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		die("Could not create socket");

	node.sin_addr.s_addr = ipaddress[9];
	node.sin_family = AF_INET;
	node.sin_port = htons(port);

	if (connect(sock, (struct sockaddr *)&node, sizeof(node)) < 0) {
		die("Connect to node failed");
	}


	puts("Connected to node\n\n");

	free(ipaddress);

	return sock;
}

// Connects node to the server

int connect_to_server(int listenport, int port, char ipaddress[11]){

	int sock;
	struct sockaddr_in server;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		die("Could not create socket");

	server.sin_addr.s_addr = inet_addr(ipaddress);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
		die("Connect failed");

	//HANDSHAKE
	unsigned char temp[2];
	recv(sock, temp, 2, 0);
	uint16_t serverShake = temp[0] * 256 + temp[1];
	if (serverShake != 16703){
		die("Received a wrong handshake");
	}

	unsigned char clientShake[2];
	clientShake[0] = DHT_CLIENT_SHAKE >> 8;
	clientShake[1] = DHT_CLIENT_SHAKE & 0xff;
	send(sock, clientShake, 2, 0);

	//CREATING HASH
    uint16_t addrlen = strlen((char*)inet_ntoa(server.sin_addr)) + 2;
    unsigned char* addrtemp = (unsigned char*)inet_ntoa(server.sin_addr);
    address[0] = listenport >> 8;
    address[1] = listenport & 0xff;
    address[addrlen] = '\0';
    for (int i=2; i<addrlen; i++) {
      address[i] = *addrtemp;
      addrtemp++;
    }

    SHA1(address, strlen((char*)address), hash);


    send_packet(sock, DHT_REGISTER_BEGIN, hash, hash, addrlen, address);

	printf("Connected to the server\n\n");

    return sock;
}

//Connects node to its java part

int connect_to_java(int listenport, int port, char ipaddress[11]){

	int sock;
	struct sockaddr_in server;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		die("Could not create socket");

	server.sin_addr.s_addr = inet_addr(ipaddress);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
		puts("JAVA NOT CONNECTED!!!!!!");

	printf("Connected to JAVA\n\n");

    return sock;
}

//Saves incoming data

int set_data(unsigned char* target, uint16_t* length, unsigned char* payload) {

	file b;

	b.key = malloc(20);
	memcpy(b.key, target, 20);
	b.data = malloc(*length);
	memcpy(&b.size, length, 4);
	memcpy(b.data, payload, *length);
	b.removed = 0;

	filearray[no_of_files] = b;

	no_of_files++;

	if (no_of_files == arraysize) { // if array is full, double it's size
		arraysize = 2 * arraysize;
		filearray = dbl_array(filearray, arraysize * sizeof(*filearray));
	}

	return 1;
}

//Thread that receives packets from server

void* runner(){
	while(1){
		receive_packet(*connectsock);

	    if (type == DHT_REGISTER_FAKE_ACK){
	    	send_packet(*connectsock, DHT_REGISTER_DONE, sender, sender, 0, '\0');
	    }

	    else if (type == DHT_REGISTER_BEGIN){

	    	/* NEW NODE JOINS - check if it needs some of this nodes data
	    	 * DOESN'T WORK AS IT SHOULD - SOMETHING IS WRONG WITH COMPARING THE DISTANCES :(
	    	 */

	    	for (int i=0;i<no_of_files;i++) {
	    		if (abs(memcmp(hash, filearray[i].key, 20)) > abs(memcmp(sender, filearray[i].key, 20)) && filearray[i].removed == 0) {
	    			int node = connect_to_node(payload);
	    			send_packet(node, DHT_TRANSFER_DATA, filearray[i].key, hash, filearray[i].size, filearray[i].data);
	    			close(node);
	    			filearray[i].removed = 1;
	    			}
	    		else if (abs(memcmp(hash, filearray[i].key, 20)) == abs(memcmp(sender, filearray[i].key, 20)) && filearray[i].removed == 0) {
	    			if (memcmp(sender, hash, 20) > 0) {
		    			int node = connect_to_node(payload);
		    			send_packet(node, DHT_TRANSFER_DATA, filearray[i].key, hash, filearray[i].size, filearray[i].data);
		    			close(node);
		    			filearray[i].removed = 1;
	    			}
	    		}
	    	}

	    	int node = connect_to_node(payload);
    		send_packet(node, DHT_REGISTER_ACK, hash, hash, 0, '\0');
    		close(node);
	    }

	    else if(type == DHT_REGISTER_DONE){
	    	// upon receiving DHT_REGISTER_DONE node removes files that it should
	    	printf("Register done\n\n");
	    	for (int i=0;i<no_of_files;i++) {
	    		if (filearray[i].removed) {
	    			no_of_files--;
	    			for (int j=i;j<no_of_files;j++){
	    				filearray[j]=filearray[j+1];
	    			}
	    		}
	    	}
	    }

	    else if(type == DHT_DEREGISTER_DENY){
	    	printf("Deregister denied\n\n");
	    }

	    else if(type == DHT_DEREGISTER_ACK){

	    	unsigned char* TCPaddress1;
	    	TCPaddress1 = malloc(11);

	    	unsigned char* TCPaddress2;
	    	TCPaddress2 = malloc(11);

	        memcpy(TCPaddress1, payload, 11);
	        memcpy(TCPaddress2, payload + 11, 11);

	        SHA1(TCPaddress1, 11, left); // left and right are hashes of neighbours, left is not necessarily left.
	        SHA1(TCPaddress2, 11, right);

	        for (int i=0;i<no_of_files;i++) {
	        	if (abs(memcmp(left, filearray[i].key, 20)) < abs(memcmp(right, filearray[i].key, 20))) {
	        		int node = connect_to_node(TCPaddress1);
	        		send_packet(node, DHT_TRANSFER_DATA, filearray[i].key, filearray[i].key, filearray[i].size, filearray[i].data);
	        		close(node);
	        	}

	        	else {
	        		int node = connect_to_node(TCPaddress2);
	        		send_packet(node, DHT_TRANSFER_DATA, filearray[i].key, filearray[i].key, filearray[i].size, filearray[i].data);
	        		close(node);
	        	}
	        }
	        int node = connect_to_node(TCPaddress1);
	        send_packet(node, DHT_DEREGISTER_BEGIN, hash, hash, 0, '\0');
	        close(node);
	        node = connect_to_node(TCPaddress2);
	        send_packet(node, DHT_DEREGISTER_BEGIN, hash, hash, 0, '\0');
	        close(node);
	    }

	    else if(type == DHT_DEREGISTER_DONE){

	    	receive_packet(*connectsock);
	    	if (type == 13){
	    		printf("Disconnect successful\n\n");
	    	}
	    }

	    else if(type == DHT_GET_DATA){

	    	int data_found = 0; // if not found, send DHT_NO_DATA

	    	int node = connect_to_node(payload);

	    	for (int i=0;i<no_of_files;i++){
	    		if (memcmp(filearray[i].key, target, 20) == 0){
	    			send_packet(node, DHT_SEND_DATA, hash, hash, filearray[i].size, filearray[i].data);
	    			data_found = 1;
	    		}
	    	}
	    	if (data_found == 0){
	    		send_packet(node, DHT_NO_DATA, hash, hash, 0, '\0');
	    	}

	    	close(node);
	    }

	    else if(type == DHT_PUT_DATA){
	    	// call for set_data
	    	if (set_data(target, length, payload)) {
	    		send_packet(*connectsock, DHT_PUT_DATA_ACK, target, sender, 0, '\0');
	    	}
	    	else {
	    		send_packet(*connectsock, DHT_PUT_DATA_ACK, sender, sender, 0, '\0');
	    	}

	    }

	    else if (type == DHT_DUMP_DATA){

	    	for (int i = 0; i < no_of_files; i++){
	    		if (memcmp(filearray[i].key, target, 20) == 0){
	    			puts("DUMP");
	    			no_of_files--;
	    			for (int j=i;j<no_of_files;j++){
	    				filearray[j]=filearray[j+1];
	    			}

	    		}
	    	}
	    	send_packet(*connectsock, DHT_DUMP_DATA_ACK, target, sender, 0, '\0');
	    }
	    else if (type == DHT_PUT_DATA_ACK) {
	    	if (memcmp(target, jkey, 20) == 0) {
	    		send_java_packet(1);
	    	}
	    	else {
	    		send_java_packet(2);
	    	}
	    }
	    else if (type == DHT_DUMP_DATA_ACK) {
	    	send_java_packet(5);
	    }
	    else{
	    	die("Unknown packet type");
	    }
	    type = 0;

	}
    return NULL;
}
//Thread that listens for incoming connections and receives incoming packets from other nodes

void* listen2(){

    fd_set rfds;
    int retval;
	while(1){

		FD_ZERO(&rfds);
    	FD_SET(0, &rfds);
    	FD_SET(*listensock, &rfds);

    	retval = select(*listensock + 1, &rfds, NULL, NULL, NULL);


    	if (retval == -1){
    		die("select failed");
    	}

    	if (FD_ISSET(*listensock, &rfds)) {
    		struct sockaddr_in tempaddr;
    		unsigned int addrlen = 0;

    		int tempfd = accept(*listensock, (struct sockaddr *)&tempaddr,
    								&addrlen);
    		receive_packet(tempfd);

    		if (type == DHT_TRANSFER_DATA) {
    			set_data(target, length, payload);
    		}

    		else if (type == DHT_REGISTER_ACK) {
    			registeracks++;
    			if (registeracks == 2) {
    				send_packet(*connectsock, DHT_REGISTER_DONE, hash, hash, 0, '\0');
    				registeracks = 0;
    			}

    		}
    		else if(type == DHT_DEREGISTER_BEGIN) {
    			send_packet(*connectsock, DHT_DEREGISTER_DONE, sender, hash, 0, '\0');
    		}
    		else if (type == DHT_SEND_DATA) {
    			send_java_packet(3);
    		}
    		else if (type == DHT_NO_DATA) {
    			send_java_packet(4);
    		}
    		close(tempfd);
    	}

    }
	return NULL;
}

// Thread function, receives packets from java part

void* javaread() {

	while(1){


		receive_javapacket(*javasock);
		// JAVA PACKET - UPLOAD
		if (jtype == 1) {
			send_packet(*connectsock, DHT_PUT_DATA, jkey, hash, *jlength, jvalue);
		// JAVA PACKET - DOWNLOAD
		}
		else if (jtype == 2) {
			send_packet(*connectsock, DHT_GET_DATA, jkey, hash, 11, address);
		// JAVA PACKET - DUMP
		}
		else if (jtype == 3){
			send_packet(*connectsock, DHT_DUMP_DATA, jkey, hash, 0, '\0');
		}
		else {
			puts("WRONG JAVA TYPE");
		}
    }
	return NULL;
}

// main function which connects/disconnects node to/from server and gives node information
int main(void) {

	no_of_files = 0;



    int listenport;
	int port;
	int javaport;
    char cmd;
    char *ipaddress;

    ipaddress = malloc(11);
    listensock = malloc(sizeof(int));
    hash = malloc(20);
    address = malloc(11);
    target = malloc(20);
    sender = malloc(20);
    request_type = malloc(4);
    length = malloc(2);
    jlength = malloc(2);
    jkey = malloc(20);
    connectsock = malloc(sizeof(int));
    javasock = malloc(sizeof(int));
    jlength2 = malloc(2);
    left = malloc(20);
    right = malloc(20);

    for(int i = 0; i<20; i++){
    	nullhash[i] = 0;
    }

    uint16_t ff = 255;

    for (int i=0;i<20;i=i+2){
    	max[i] = ff >> 8;
    	max[i+1] = ff & 0xff;
    }

    arraysize = 2;
    filearray = malloc(arraysize * sizeof(*filearray));

    while(1){

        //Commands are c, d, i, e

    	puts("c = connect to server");
    	puts("d = try disconnect from server");
    	puts("i = information");
    	puts("e = exit while loop and close socket");
        puts("Give command:");
        scanf("%s", &cmd);
        printf("\n");

        if (cmd == 'c'){
            printf("Give listen port:\n");
            scanf("%d", &listenport);
            printf("\n");

            printf("Give java port:\n");
            scanf("%d", &javaport);
            printf("\n");


            printf("Give servers IP-address:\n");
            //scanf("%s", ipaddress);
            ipaddress = IPADDRESS;
            printf("\n");

            printf("Give servers port:\n");
            //scanf("%d", &port);
            port = 6666;
            printf("\n");

            *listensock = create_listen_socket(listenport);

            *connectsock = connect_to_server(listenport, port, ipaddress);

            *javasock = connect_to_java(listenport, javaport, ipaddress);

            pthread_t run_thread, listen_thread, java_thread;

            pthread_create(&listen_thread, NULL, &listen2, NULL);

            pthread_create(&run_thread, NULL, &runner, NULL);

            pthread_create(&java_thread, NULL, &javaread, NULL);
        }

        else if (cmd == 'd'){
        	send_packet(*connectsock, DHT_DEREGISTER_BEGIN, hash, hash, 0, '\0');
        }

        else if(cmd == 'i'){
        	puts("Information:\n");

        	printf("\nOwn hash: ");
            for (int i = 0; i < 20; i++){
            	printf("%x", hash[i]);
            }

            printf("\nNUMBER OF FILES: %d\n", no_of_files);

            printf("\nDATA:");
            for (int i=0;i<no_of_files;i++){
            	printf("\n");
            	for (int j=0;j<20;j++){
            		printf("%x", (filearray[i].key)[j]);
            	}
            }

            printf("\n\n");
        }
        else if(cmd == 'e'){
        	break;
        }
        sleep(1);
    }
    close(*listensock);
    close(*connectsock);
    close(*javasock);
    free(listensock);
    free(hash);
    free(address);
    free(target);
    free(sender);
    free(request_type);
    free(length);
    free(connectsock);
    free(javasock);
    free(filearray);
    return 0;
}
// Don't use listen ports 9999 and 8888
