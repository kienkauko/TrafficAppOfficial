#include "all_lib.hpp"
#include "density.hpp"

#define SO_LAN_DO 10
#define TIMEOUT 10
#define BILLION 1E9
#define PORT1 9001
#define PORT2 9002

int av_density_arr[SO_LAN_DO];
int lan_do = 0;

float32_t muy[307200], var[307200];
uint8_t bin[307200]; 
uint8_t gray1f[307200];
int confirm_bg = 0;
int *pconfirm_bg = &confirm_bg;
int check_recv = 0;
int check_send_lando = 0, check_send_density = 0, check_starttime = 0;
uint32_t av_density;
uint32_t *p_av_density = &av_density;
int frame_count = 0;
double starttime = 0;
double fixedtime = 0;
double starttime_d = 0; double endtime_d = 0;
struct timeval tv_main = {0};

void density(){
	av_density = findDensity_test(gray1f, muy, var, bin);

	  // -------------------- Clear vector --------------------------------
    frame_count++;
	cout << "Frame: " <<frame_count << endl;
    cout << '-' << "Density: " << av_density << '%' << endl;
}

int main(int argc, char* argv[])
{
        
		//lan do so?
		int lan_do_so = 0;
		int *p_lan_do_so = &lan_do_so;

		//thoi gian thuc hien
		float difference = 0;
		float *p_difference=&difference;
		
		//khoi tao socket de nhan du lieu
	  int socket_desc , new_socket , c; //receive data
	  int network_socket;		// send data
      struct sockaddr_in server , client;
	  struct timespec end_p; //measure time to process
	  struct timespec start_d, end_d; //measure time of disconnection
      //Create socket
      socket_desc = socket(AF_INET , SOCK_STREAM , 0);
      if (socket_desc == -1)
      {
         printf("Could not create socket");
      }
      
    
      cout << "You have entered " << argc 
         << " arguments:" << "\n"; 
  
    for (int i = 0; i < argc; ++i) 
        cout << argv[i] << "\n"; 
  
    //return 0;
      
      //Prepare the sockaddr_in structure
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = htons(PORT1);

      //Bind
     if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
     {
       puts("bind failed");
       return -1;
     }

     puts("bind done");

     //Listen
    listen(socket_desc , 3);
    puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
    fflush(stdout);
    ///create socket for sending //////////////////
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (network_socket < 0) 
		printf("Error when create socket\n");
	else printf("Successful create socket\n");
	
	// specify an address for the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT2);
	
	
	int connection_status = 0;
	do{
		server_address.sin_addr.s_addr = inet_addr(argv[1]);
		connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
		
		sleep(1);
	}while(connection_status == -1);

    /****************************************************************/
    ////nhan lien tuc////////////////////////////
	new_socket = accept(socket_desc, (struct sockaddr *)&server,(socklen_t*)&c);
    
    while(1){
    		///start receiving background innitiation/////////
    		if (confirm_bg == 0){
    			recv(new_socket, muy, 307200, MSG_WAITALL);
    			recv(new_socket, var, 307200, MSG_WAITALL);
    			cout << "Done background initiation \n";
				confirm_bg = 1; //confirm that background initiation has been completed.
    		}
    				
    		recv(new_socket, &starttime, 8, MSG_WAITALL);
    		if (lan_do == 0){ 
    			fixedtime = starttime;
    			check_starttime		=	send(network_socket, &fixedtime, 8, MSG_NOSIGNAL); // send starting time
    		}
    		cout << "Start time received: " << starttime << endl;
    		check_recv = recv(new_socket, gray1f, 307200, MSG_WAITALL);	//nhan image in  gray
    		if (check_recv == 0){
    			cout << "Client has been stopped by some reasons, waiting for reconnection ... \n";
    			new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c);
    			confirm_bg = 0;
    			continue;
    		}

//    		cout << "Evething seems fine, lando number is: " << lan_do << endl;
    		
    		density();		//chay ham density
    		
    		av_density_arr[lan_do] = av_density;
    		++lan_do;
			if (lan_do < SO_LAN_DO ) continue;
			// Neu chua do du so lan thi chua gui anh ve server
				
			av_density = 0;
			for (int i = 0; i < SO_LAN_DO; ++i){
				av_density += av_density_arr[i];
			}
				
			av_density = av_density / SO_LAN_DO;
			
			
			lan_do_so++;
//			cout << "Lan do: " << lan_do_so << endl;
//			cout << "Final density: " << av_density << "%" << endl;
			
			
			// calculate time-taken
			
			
			//connect to send lan_do_so, av_density, time process
			
			cout << "Starting sending data to LOS.\n";
			
			check_send_lando	= 	send(network_socket, p_lan_do_so, sizeof(int), MSG_NOSIGNAL);
			check_send_density	= 	send(network_socket, p_av_density, sizeof(uint32_t), MSG_NOSIGNAL);
			
			int time_to_out = 0;
			int count = 0;
			while (check_send_lando == -1 || check_send_density == -1 || check_starttime == -1){
				if(count == 0){
					clock_gettime( CLOCK_REALTIME, &start_d); // get disconnection time
					starttime_d = start_d.tv_sec + (double)(start_d.tv_nsec)/BILLION;
				}
				cout << "Disconnect to server!trying to reconnect in 2s ... \n";
				cout << "Previous packet will be dumped! \n";
				///attempt to reconnect/////////////////////////////
				network_socket = socket(AF_INET, SOCK_STREAM, 0);
					
				connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
			
				if(connection_status == -1){
					sleep(1);
					time_to_out ++;
					cout << "Reconnection status: " << connection_status << endl;
					cout << "Time left to timeout: " << TIMEOUT - time_to_out << endl;
				}
				if (connection_status != -1){
					clock_gettime( CLOCK_REALTIME, &end_d); // get reconnection time
					endtime_d = end_d.tv_sec + (double)(end_d.tv_nsec)/BILLION;
					cout << "Reconnect successfully! \n";
					break;
				}
				if(time_to_out >= TIMEOUT){
					cout << "Timeout reached, exit." << endl;
					return -1;
				}
				
   			}
   			cout << "Finish lando number: " << lan_do_so << endl;
			lan_do = 0;
			clock_gettime( CLOCK_REALTIME, &end_p);
			cout << " End time is " << end_p.tv_sec << endl;
			double difference_d = endtime_d - starttime_d;
			double difference = end_p.tv_sec  + (double)(end_p.tv_nsec)/BILLION - fixedtime;
			cout << "It took " << difference << " seconds to process " << endl;
			cout << "It took " << difference_d << " seconds to reconnect " << endl;
    	}
    close(network_socket);
    return 0;
}
