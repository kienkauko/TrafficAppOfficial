#include "receive.hpp"
//unsigned char buf_vel[921600];
uint8_t gray_a[5][307200];



int receive2D(int socket){  

int col, SIZE = 307200;
int check_recv;

        for(int row = 0; row < 5; row++){
        	for ( col = 0; col < SIZE; col+=4 ){
				check_recv = recv(socket, gray_a[row] + col, 4, MSG_WAITALL);
    		}
    	}
	printf("2D successfully Received!\n");
  return check_recv;
}



    

