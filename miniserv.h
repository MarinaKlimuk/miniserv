#ifndef MINISERV_H
# define MINISERV_H

#include <sys/select.h>   // FD_ZERO, FD_SET, FD_ISSET, select()
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

fd_set afds, wfds, rfds;
extern int max_fd;
extern int count;
int		ids[65536];
char	*msgs[65536];
char	buf_read[1001], buf_write[42];


char *str_join(char *buf, char *add);
int extract_message(char **buf, char **msg);


void	err(char *msg);
void	notify_other(int author, char *str);
void	remove_client(int fd);
void	register_client(int fd);
void	send_msg(int fd);

#endif