#include "all_lib.hpp"

#define BILLION 1E9

int main(int argc, char* argv[]){
		//int i = 0;
		int lan_do_so=0;
		uint32_t av_density=0;
		float time_process=0;
		int opt = 1;
		int check_lando = 0;
		int check_density =0;
		int check_starttime = 0;
		int check_recv_time = 0;
		double starttime = 0;
		//khoi tao socket
	  int socket_desc , new_socket , c;
	  struct timespec start_p, end_p; //measure time to process
	  
      struct sockaddr_in server;
	  
      //Create socket
      socket_desc = socket(AF_INET , SOCK_STREAM , 0);
      if (socket_desc == -1){
         printf("Could not create socket");
      }
  	  int opt = 1;
      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    //return 0;
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = htons(9004);

      //Bind
     if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
       puts("bind failed");
       return -1;
     }

     puts("bind done");

     //Listen
     listen(socket_desc , 3);

      //Accept and incoming connection
      //puts("Waiting for incoming connections...");
      c = sizeof(struct sockaddr_in);
	  fflush(stdout);
    	/****************************************************************/
    	////nhan lien tuc
    new_socket = accept(socket_desc, (struct sockaddr *)&server,(socklen_t*)&c);
    puts("Connection accepted");
    
	while(1){
		clock_gettime( CLOCK_REALTIME, &start_p);
		
		check_starttime	= recv(new_socket, &starttime, 8, MSG_WAITALL);
		cout << "Start time received: " << starttime << endl;
		check_lando		= recv(new_socket, &lan_do_so, sizeof(lan_do_so),MSG_WAITALL);
		check_density	= recv(new_socket, &av_density, sizeof(av_density), MSG_WAITALL);
			
		if(check_lando == 0 || check_density == 0 || check_starttime == 0){
			cout << "Client has been stopped by some reasons, waiting for reconnection ... \n";
    		new_socket = accept(socket_desc, (struct sockaddr *)&server,(socklen_t*)&c);
    		continue;
		}
		cout << "Lan do so: " << lan_do_so << endl;
		cout << "Density: " << av_density << "%" << endl;
		
		clock_gettime( CLOCK_REALTIME, &end_p);
		
		double difference = end_p.tv_sec  + (double)(end_p.tv_nsec)/BILLION - starttime;
		double consecutive_time = end_p.tv_sec  + (double)(end_p.tv_nsec)/BILLION - (start_p.tv_sec  + (double)(start_p.tv_nsec)/BILLION);
		cout << "It took " << difference << " seconds to finish the entire process. " << endl;
		cout << "Time between 2 consecutive receives: " << consecutive_time << " seconds." << endl;
	}
	close(new_socket);
	return 0;
}
