// Packets loss test according to Mike Parkin's request
// Author: Guang Ling
// Version: v1.0
////////////////////////////////////////////////////
//
//     IMPORTANT !!!
//
// This test is JUST for some VLS-128 sensors that under suspicion of packet loss, 
// NOT for other models or some obviously defective sensors.
// Please use veloview or other tools to check the sensor first.
//
////////////////////////////////////////////////////
//
// Already tested in Mike Parkin's mini Computer. it works well.

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

void write_to_log(FILE *fp, long time_gap_per_pkg);

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
    socklen_t addrLen = sizeof(struct sockaddr_in);
    
    int azimuth_for_first_block;
    // the vls-128 outputs around 21701388 packets / hour
    // so 20,000,000 is nearly one hour
    long count = 20000000;

    long sensor_time_start;
    long sensor_time_end;
    long sensor_time;
    long time_gap_per_hour;
    long time_gap_per_pkg;
    long tmp_time = 0;

    FILE *fp;
    char fname[50] = "log_for_packet_loss_";
    time_t t;
    char time_now[20];
    sprintf(time_now, "%ld", time(&t));
    strcat(fname, time_now);
    printf("%s\n", fname);

    for (int hours=0; hours < TEST_HOUR; hours++){
        printf("Hour %d Test Start......\n", hours);
        for (long i=0; i<count; i++) {    
            if (i%60000 == 0) {
                printf("Don't worry, Test is alive..., numbers must change per 10 seconds: %ld \n", i);
            }    
            recvbytes = recvfrom(sockListen, recvbuf, 1206, 0, (struct sockaddr *)&recvAddr, &addrLen);
            if(recvbytes != -1) {
                sensor_time = recvbuf[1203]*256*256*256 + recvbuf[1202]*256*256 + recvbuf[1201]*256 + recvbuf[1200];   
                time_gap_per_pkg = sensor_time - tmp_time;
                azimuth_for_first_block = recvbuf[3]*256 + recvbuf[2];            
            } else {
                printf("recvfrom fail\n");
            }

            if (i==0) {
                sensor_time_start = sensor_time;   
            }

            if (i==(count-1)) {
                sensor_time_end = sensor_time;
            }
            tmp_time = sensor_time;
            if (i!= 0 && time_gap_per_pkg > 240) {
                fp = fopen(fname,"a+");    
                write_to_log(fp, time_gap_per_pkg);
                fclose(fp);
            }
        }

        time_gap_per_hour = sensor_time_end - sensor_time_start;
        if (time_gap_per_hour < 0) {
            time_gap_per_hour = time_gap_per_hour + 3600000000;
        }
        printf("Hour %d Test Done!......\n", hours);
    }

    return 0;
}

void write_to_log(FILE *fp, long time_gap_per_pkg) {
    char log[150] = "Packets loss detected, system time (Linux time) is: ";
    char time_now2[20];
    char time_gap[20];
    time_t t;
    sprintf(time_now2, "%ld", time(&t));
    strcat(log, time_now2);
    char log2[50] = " , sensor time gap should be 160 us, now is: ";
    strcat(log, log2);
    sprintf(time_gap, "%ld", time_gap_per_pkg);
    strcat(log, time_gap);
    strcat(log, "\n");
    fwrite(log, sizeof(char), 150, fp);
    printf("%s\n", log);

}
