#include "miniserv.h"

int max_fd = 0;
int count = 0;

int main (int argc, char **argv){
    if (argc!=2)
        return (-1);
    
    //1. CLEAR THE FOLDER WITH FILE DESCRIPTORS
    //afds is a variable of the fd_set type
    //afds contains all file descriptors
    //it's important to use FD_Zero before every run to clean afds
    FD_ZERO(&afds);

    //2.CREATE A SOCKET
    //socket is like a phone throuh which a programm will communicate
    //AF_INET means that I will use regual IPv4 adresses
    //SOCK_STREAM means that I will use TCP connection
    //0 means "choose the type youself: what is right for this exact AF_INET+SOCK_STREAM pair"
    //this function will create a socket-object , but it won't give me the rights for the object
    // this function will give me a number through which ì can connect to the socket
    //probably it will return 3, because 1 = stdin, 1 stdout, 2 stderr - they are reserved
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        return (-1);

    //max_fd is metter because select() needs to know the highrst fd number to scan
    max_fd = sockfd;

    //3.ADD SOCKET TO A FOLDER
    //FD_SET adds sockfd to &afds. Now there is one descriptor in afds
    FD_SET(sockfd, &afds);


    //4.CREATE SERVADDR FOR SOCKET
    //struct sockaddr_in is a special structure which contains server adress(ip-adress and port)
    //where server will listen for connections
    //the example of the structure:
    // struct sockaddr_in {
    //     short          sin_family;   // семейство адресов, например AF_INET
    //     unsigned short sin_port;     // номер порта
    //     struct in_addr sin_addr;     // IP-адрес
    //     char           sin_zero[8];  // заполнитель, не используется
    // };
    struct sockaddr_in servaddr;
    //the whole serveraddr struct will contain zeroes
    bzero(&servaddr, sizeof(servaddr));

    //AF_INET means that I will use regual IPv4 adresses
    servaddr.sin_family = AF_INET;
    //user put as an input the number of a port. htons change it to computer format
    //htons = host to network short
    servaddr.sin_port = htons(atoi(argv[1]));
    //INADDR_LOOPBACK means that will answer only programs which run from the same computer (127.0.0.1)
    //htonl will change the number to a web-format
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // 5. CONNECT SOCKET TO SERVADDR
    //bind connects socket to the adress
    //(const struct sockaddr *) &servaddr it's a technical change sockaddr to the right type for the bid function 
    // sizeof - bind function needs to know how much data it nedds to read
    if(bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)))
        err("Fatal error\n");

    //6. MAKE SOCKET TO GET READY TO TAKE CONNECTIONS
    //SOMAXCON - how many clients can wait for a connection
    if(listen(sockfd, SOMAXCONN))
        err("Fatal error\n");


    while (1){
        //rfds and wfds are copies from afds
        //rfds - which fds are ready to read? who wants to say me smth?
        //wfds - which fds are ready to write? who wants to listen me?
        rfds = wfds = afds;


        //7. WAIT FOR SOME FD TO BE READY TO CONNECT
        // untill that moment the programm blocks. It will unblock when something will try to connect or send a message or disconnect
        //select waits untill at least one file descriptor will be ready to read or write
        //max_fd+1 - how many fds are needed to be checked
        //&rfds - set of read-fds who needs to be followed
        //&wfds - set of write-fds who needs to be followed 
        if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0)
            err("Fatal error\n");

        //8. FOR EACH FD BEING MONITORED
        //checking each file descriptor
        for (int fd = 0; fd <= max_fd; fd++){
            //9.IF IT'S NOT READY, SKIP
            //if a file descriptor is not ready for reading, skip it = ïf fd is not in the &rfds set, skip it
            if(!FD_ISSET(fd, &rfds))
                continue;

            //10. IF FD IS READY
            // if fd!=sockfd it means that it's some other, already-connected client
            //it means that a new client is trying to connect
            if(fd == sockfd){
                //11. IT'S A NEW CONNECTION REQUEST, ACCEPT IT
                //addr_len is needed for accept because accept needs to know how much space is avaliable in servaddr
                socklen_t addr_len = sizeof(servaddr);
                // accept will return a fd number (create a new socket) dedicated to talking with that specific client
                int client_fd = accept(sockfd, (struct sockaddr *)&servaddr, &addr_len);
                if(client_fd >= 0){
                    register_client(client_fd);
                    break;
                }
            }

            else{
                //12. IT'S DATA FROM AN EXISTING CLIENT, READ IT
                //recv reads incoming data from socket fd into buf_read
                int read_bytes = recv(fd, buf_read, 1000, 0);
                //check for disconnect or error. if read_bytes == 0 it means that the client closed the session
                if(read_bytes <=0){
                    remove_client(fd);
                    break;
                }
                buf_read[read_bytes] = '\0';
                msgs[fd] = str_join(msgs[fd], buf_read);
                send_msg(fd);
                
            }
        }
    }
    return (0);

}

//SUMMARY: HOW DOES IT WORK
//Server creates socket(it's like a telephone) and listens through it for connections
//When in endless while-loop it is waiting untill somebody will ask for connection or say something
//If someaone wants to connect, server creates a new socket for a client
//If some client wants to talk, it processes the message