#include<stdio.h> // Standard Input Output Library
#include<stdlib.h> // For exit and memory alloc
#include<string.h> // String operations ofc
#include<unistd.h> // POSIX OS API
#include<arpa/inet.h> // Manipulating IP ADDR and networking stuffs
#include<sys/socket.h> //Something to do with sockets


//Function for creating and configuring Sockets.
//MAN OTHER LANGUAGES DIDNT NEED TO DO THIS MUCH CONF.
int create_socket(const char *hostname, int port) {
    //Var to hold socket file descriptor.
    //AF_INET = IPv4, SOCK_STREAM = TCP. 0 IDK
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr; //STRUCT TO STORE SERVER ADDRESS INFO

    // FAILED CREATING SOCKET ? exit
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr)); //INIT server_add struct
    server_addr.sin_family = AF_INET; //setting address family to IPv4
    server_addr.sin_port = htons(port); //Sets port no. 
    //htons() converts port number to network byte order

    //inet() converts IP adresss from text to binary and stores it in server_addr.sin_addr
    // Check if conversion failed
    if (inet_pton(AF_INET, hostname, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    //connect() connects socket to server using addr info in server_info
    //Check if connectioon failed
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

//Function to send an HTTP request
void send_http_request(int sockfd, const char *path) {
    char request[1024]; //Arry to hold request String
    //snprintf() formats and stores HTTP GET request into request buffer
    snprintf(request, sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "HOST: example.com\r\n"
        "Connection: close\r\n"
        "\r\n", path);
    
    //send() sends the HTTP request over the socket.
    //Checks if send operation failed
    if(send(sockfd, request, strlen(request), 0) < 0) {
        perror("send");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

//Function to receive and print HTTP response
void receive_http_response(int sockfd) {
    char buffer[4096]; //Array to hold received data
    ssize_t bytes_received; //Variable to store the number of bytes received
    
    //Receives data from the socket in a loop untill no more data is availabe
    //recv() reads only as much as data as the size of buffer
    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) -1, 0))) {
        buffer[bytes_received] = '\0'; //Null-terminate the buffer
        printf("%s", buffer);
    }

    if (bytes_received < 0) {
        perror("recv");
    }
}

//function to close socket
void close_socket(int sockfd){
    close(sockfd);
}


// EXECUTION START POINT
int main() {
    const char *hostname = "93.184.216.34"; //example.com IP Address
    const char *path = "/"; //Root Path
    int port = 80; //HTTP default Port

    int sockfd = create_socket(hostname, port);
    send_http_request(sockfd, path);
    receive_http_response(sockfd);
    close_socket(sockfd);

    return 0;
}