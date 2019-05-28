#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

int main() {

    int sockListen = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockListen == -1) {
        printf("socket fail\n");
        return -1;
    }

    int set = 1;
    setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));
    struct sockaddr_in recvAddr;
    memset(&recvAddr, 0, sizeof(struct sockaddr_in));
    
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(2368);
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    int sockBind = bind(sockListen, (struct sockaddr *)&recvAddr, sizeof(struct sockaddr));

    if(sockBind == -1) {
        printf("bind fail\n");
        return -1;
    }

    int recvbytes;
    char recvbuf[1206];
    // Caution: if Linux, use "int" to declare addrLen, my laptop is Apple MAC
    // That's why I used socket_t
    socklen_t addrLen = sizeof(struct sockaddr_in);

    while (1) {

        if((recvbytes = recvfrom(sockListen, recvbuf, 1206, 0,
            (struct sockaddr *)&recvAddr, &addrLen)) != -1) {
            recvbuf[recvbytes] = '\0';
            printf("receive a broadCast messgse:%s\n", recvbuf);
        } else {
            printf("recvfrom fail\n");
        }
    }

    close(sockListen);

    return 0;
}

