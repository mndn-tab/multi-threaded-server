/*
run ./server
on docker click the button "open port" -> in the popup type "18000" for port
then server and chrome will send and recieve messages
*/
#include "common.h"
#include<string.h>

int main(int argc, char ** argv)
{
    int listenfd, connfd, n;
    struct sockaddr_in servaddr;
    uint8_t buff[MAXLINE + 1];
    uint8_t recvline[MAXLINE + 1];

    //create a socket(from internet family, stream socket, use TCP(==0))    
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
         err_n_die("socket error");
    
    // Im listening on port SERVER_PORT = 18000
    // and listen to INADDR_ANY = any client that wants to connect !
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);
    
    // reserves the connection, associate socket with address
    if((bind(listenfd, (SA *)&servaddr, sizeof(servaddr))) < 0 )
         err_n_die("socket error");
    
    if((listen(listenfd, 10)) < 0)
         err_n_die("socket error");
    
    //loop to accept connection and respond
    for(;;)
    {
        struct sockaddr_in addr;
        socklen_t addr_len;   
        //accept blocks until an incoming connection arrives
        // accept returns a "file descrptor " to the connection
        //IMPORTANT NOTE: "connfd" = descriptor for each client, 
        //IMPORTANT NOTE: "listenfd" = descriptor that the server is listening on
        //IMPORTANT NOTE: so we can listen to others while we are talking to this client
        printf("waiting for a connection on port %d \n", SERVER_PORT);
        fflush(stdout);
        connfd = accept(listenfd, (SA *)NULL, NULL);
        
        //zero out the reciver buffer to make sure it ends up null terminated
        memset(recvline, 0, MAXLINE);
        
        //now read the client's message
        if( (n = read(connfd, recvline, MAXLINE - 1)) > 0)
        {
               fprintf(stdout, "\n%s\n\n%s", bin2hex(recvline, n), recvline);
               
               //ends connection 
               if(recvline[n - 1] == "\n")
                      break;  
               // zero out the buffer to make sure whatever we read is null terminated   
               memset(recvline, 0 , MAXLINE);
        }
        if(n < 0) 
             err_n_die("read error");
        
        //send a response- Here just prints OK 
        snprintf( (char *)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello");
        
        //normally, you want to check write and close. Here we ignore them 
        write(connfd, (char *)buff, strlen((char *)buff));
        close(connfd);
        
    }
}
