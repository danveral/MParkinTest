// packets loss test according to Mike Parkin's request
// Author: Guang Ling
// Version: v1.0
////////////////////////////////////////////////////
//
//     IMPORTANT !!!
//
// This Test is for some VLS-128 sensors that under suspicion of packet loss, 
// NOT for sensors had obvious problems.
// use veloview or other tools to check the sensor first.
//
////////////////////////////////////////////////////

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

int sniff_from_udp(int sockListen, struct sockaddr_in recvAddr);

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
    
    sniff_from_udp(sockListen, recvAddr);
    close(sockListen);

    return 0;
}


int sniff_from_udp(int sockListen, struct sockaddr_in recvAddr) {
   
    int recvbytes;
    char recvbuf[1206];
    // Caution: if Linux, use "int" to declare addrLen, that depends on your computer
    // in Mac or some other device, you may use "socklen_t addrLen"
    int addrLen = sizeof(struct sockaddr_in);
    
    int azimuth_for_first_block;
    // the vls-128 outputs around 21701388 packets / hour
    // so 20,000,000 is nearly one hour
    long count = 2000;

    long sensor_time_start;
    long sensor_time_end;
    long sensor_time;
    long time_gap;

    for (int hours=0; hours < TEST_HOUR; hours++){
        for (long i=0; i<count; i++) {        
            recvbytes = recvfrom(sockListen, recvbuf, 1206, 0, (struct sockaddr *)&recvAddr, &addrLen);
            if(recvbytes != -1) {
                sensor_time = recvbuf[1203]*256*256*256 + recvbuf[1202]*256*256 + recvbuf[1201]*256 + recvbuf[1200];   
                azimuth_for_first_block = recvbuf[3]*256 + recvbuf[2];            
            } else {
                printf("recvfrom fail\n");
            }

            if (i==0) {
                sensor_time_start = recvbuf[1203]*256*256*256 + recvbuf[1202]*256*256 + recvbuf[1201]*256 + recvbuf[1200];   
            }

            if (i==(count-1)) {
                sensor_time_end = recvbuf[1203]*256*256*256 + recvbuf[1202]*256*256 + recvbuf[1201]*256 + recvbuf[1200];
            }
        }

        time_gap = sensor_time_end - sensor_time_start;
        if (time_gap < 0) {
            time_gap = time_gap + 3600000000;
        }
        printf("%ld\n", time_gap);
    }

    return 0;
}
