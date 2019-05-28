// packets loss test according to Mike Parkin's request
// Author: Guang Ling
// Version: v1.0

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
#include <time.h>

#define TEST_HOUR 5  // Input how many hours do you want to test.

int main() {

    int udp_des_port = 2368;
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
    recvAddr.sin_port = htons(udp_des_port);
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    int sockBind = bind(sockListen, (struct sockaddr *)&recvAddr, sizeof(struct sockaddr));

    if(sockBind == -1) {
        printf("bind fail\n");
        return -1;
    }

    int recvbytes;
    char recvbuf[1206];
    // Caution: if Linux, you may use "int" to declare addrLen, that depends on your computer
    socklen_t addrLen = sizeof(struct sockaddr_in);
    
    int azimuth_for_first_block;
    // the vls-128 output 21701388 packets / hour
    for (int i=0; i<21701389; i++) {
        if((recvbytes = recvfrom(sockListen, recvbuf, 1206, 0,
            (struct sockaddr *)&recvAddr, &addrLen)) != -1) {
            //recvbuf[recvbytes] = '\0';
            //printf("receive a broadCast messgse:%s\n", recvbuf);
            azimuth_for_first_block = recvbuf[3]*256 + recvbuf[2];            
            printf("%d\n",azimuth_for_first_block);
        } else {
            printf("recvfrom fail\n");
        }
    }

    close(sockListen);

    return 0;
}

