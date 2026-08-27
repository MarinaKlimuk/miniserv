#include "miniserv.h"


//THIS IS THE SAME MAIN AS IN MINISERV.C, BUT WITHOUT COMMENTS

fd_set afds, wfds, rfds;
int max_fd = 0;
char	buf_read[1001], buf_write[42];

int main (int argc, char **argv){
    if (argc!=2)
        return ("Wrong amount of arguments", -1);
    

    FD_ZERO(&afds);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        return ("Fatal error\n", -1);
    max_fd = sockfd;
    FD_SET(sockfd, &afds);


    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[1]));
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if(bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)))
        err("Fatal error\n");
    if(listen(sockfd, SOMAXCONN))
        err("Fatal error\n");


    while (1){

        rfds = wfds = afds;

        if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0)
            err("Fatal error\n");

r
        for (int fd = 0; fd <= max_fd; fd++){
            if(!FD_ISSET(fd, &rfds))
                continue;

            if(fd == sockfd){
                socklen_t addr_len = sizeof(servaddr);
                int client_fd = accept(sockfd, (struct sockaddr *)&servaddr, &addr_len);
                if(client_fd >= 0){
                    register_client(client_fd);
                    break;
                }
            }

            else{
                int read_bytes = recv(fd, buf_read, 1000, 0);
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