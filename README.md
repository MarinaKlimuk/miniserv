# miniserv

### HOW TO RUN THE PROGRAM

1. Compile
c -Wall -Wextra -Werror miniserv.c helpers.c

2. Open 3 terminals
    1. in 1st terminal type: ./a.out 8080
    2. in 2nd terminal type: nc 127.0.0.1 8080
    3. in 3rd terminal type: nc 127.0.0.1 8080

    nc simulates client, the programm contains only server part

3. Communicate: send messages in different terminals

4. To disconnect type: ctrl+c or ctrl+d



### SUMMARY: HOW DOES IT WORK

Server creates socket(socket is like a telephone) and listens through it for connections
When in endless while-loop it is waiting untill somebody will ask for connection or say something
If someaone wants to connect, server creates a new socket for a client
If some client wants to talk, it processes the message