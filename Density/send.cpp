#include "send.hpp"
#include <iostream>


int sendUint32(int socket, uint32_t*  data, int SIZE){
        int i;

        for (i = 0; i < SIZE; i = i + 4){  
            write(socket, &data[i], 4);   // write 4 bytes every time
        }
}
int sendInt(int socket, int*  data, int SIZE){
        int i;

        for (i = 0; i < SIZE; i = i + 4){  
            write(socket, &data[i], 4);   // write 4 bytes every time
        }
}

int sendTime(int socket, float*  data, int SIZE){
        int i;
        
        for (i = 0; i < SIZE; i = i + 4){  
            write(socket, &data[i], 4);   // write 4 bytes every time
        }
}


