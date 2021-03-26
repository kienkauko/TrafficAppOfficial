#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>   
#include<unistd.h>  
#include<iostream>
   //#include <sstream>
#include<fstream>
#include<errno.h>
#include <arm_neon.h>

//int sendChar(int socket, unsigned char* data, int SIZE);
//int sendFloat(int socket, float32_t*  data, int SIZE);
//int sendUint(int socket, uint8_t*  data, int SIZE);
int sendUint32(int socket, uint32_t*  data, int SIZE);
int sendInt(int socket, int*  data, int SIZE);
int sendTime(int socket, float*  data, int SIZE);
