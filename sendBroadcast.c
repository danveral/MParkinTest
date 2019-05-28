// Just for sending the broadcast

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

int send_a_UDP_pkg(int brdcFd, struct sockaddr_in theirAddr); 

int main() { 

    int brdcFd;

    // define a socket
    if((brdcFd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("socket fail\n");
        return -1;
    }

    int optval = 1;
    setsockopt(brdcFd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));
    struct sockaddr_in theirAddr;
    memset(&theirAddr, 0, sizeof(struct sockaddr_in));

    theirAddr.sin_family = AF_INET;
    theirAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    theirAddr.sin_port = htons(2368);

    for (int i=0; i<5; i++) {
        send_a_UDP_pkg(brdcFd, theirAddr);
        usleep(160);
    }

    close(brdcFd);
    return 0;
}

int send_a_UDP_pkg(int brdcFd, struct sockaddr_in theirAddr) {
    char msg[1206] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    int sendBytes;
    if((sendBytes = sendto(brdcFd, msg, strlen(msg), 0,
            (struct sockaddr *)&theirAddr, sizeof(struct sockaddr))) == -1) {
        printf("sendto fail, errno=%d\n", errno);
        return -1;
    }

    printf("msg=%s, msgLen=%zu, sendBytes=%d\n", msg, strlen(msg), sendBytes);

    return 0;
}
