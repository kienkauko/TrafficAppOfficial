#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>   
#include<unistd.h>  
#include<iostream>
#include <sstream>
#include<fstream>
#include<errno.h>
#include <arm_neon.h>

using namespace std;
//extern unsigned char buf_vel[921600];
extern uint8_t gray_a[5][307200];

int receive2D(int socket);

