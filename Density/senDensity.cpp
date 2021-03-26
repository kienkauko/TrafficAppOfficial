#include "all_lib.hpp"
#include "density.hpp"

#define SO_LAN_DO 10
#define TIMEOUT 120
#define BILLION 1E9

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
struct timeval tv_main = {0};
double endtime = 0;

void density(){
	av_density = findDensity_test(gray1f, muy, var, bin);

	  // -------------------- Clear vector --------------------------------
    frame_count++;
	cout << "Frame: " <<frame_count << endl;
    cout << '-' << "Density: " << av_density << '%' << endl;
}

int main()
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
	  struct timespec end_p, start_p; //measure time to process
      //Create socket
      socket_desc = socket(AF_INET , SOCK_STREAM , 0);
      if (socket_desc == -1)
      {
         printf("Could not create socket");
      }
      
 
      //Prepare the sockaddr_in structure
      server.sin_family = AF_INET;
      server.sin_addr.s_addr = INADDR_ANY;
      server.sin_port = htons(9003);

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
	server_address.sin_port = htons(9004);
	
	
	int connection_status = 0;
	do{
		server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
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
		
                clock_gettime(CLOCK_REALTIME, &start_p);
    		recv(new_socket, &starttime, 8, MSG_WAITALL);

    		if (lan_do == 0){ 
    			fixedtime = starttime;
    			check_starttime		=	send(network_socket, &fixedtime, 8, MSG_NOSIGNAL); // send starting time
    		}
    		cout << "Start time received: " << starttime << endl;
    		check_recv = recv(new_socket, gray1f, 307200, MSG_WAITALL);	//nhan image in  gray
    		
		clock_gettime( CLOCK_REALTIME, &end_p);
                double difference1 = end_p.tv_sec - start_p.tv_sec  + (double)(end_p.tv_nsec - start_p.tv_nsec)/BILLION;
                 cout << "It took " << difference1 << " seconds to receive gray image " << endl;

		if (check_recv == 0){
    			cout << "Client has been stopped by some reasons, waiting for reconnection ... \n";
    			new_socket = accept(socket_desc, (struct sockaddr *)&client,(socklen_t*)&c);
    			confirm_bg = 0;
    			continue;
    		}

    		//cout << "Evething seems fine, lando number is: " << lan_do << endl;
    		clock_gettime(CLOCK_REALTIME, &start_p);
    		density();		//chay ham density
    		
			clock_gettime( CLOCK_REALTIME, &end_p);
            double difference4 = end_p.tv_sec - start_p.tv_sec  + (double)(end_p.tv_nsec - start_p.tv_nsec)/BILLION;
            cout << "It took " << difference4 << " seconds to calculate each density " << endl;

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
			cout << "Lan do: " << lan_do_so << endl;
			cout << "Final density: " << av_density << "%" << endl;
			
			
			// calculate time-taken
			
			
			//connect to send lan_do_so, av_density, time process
			
			//cout << "Starting sending data to final boss \n";
			
			clock_gettime( CLOCK_REALTIME, &end_p);
                double difference2 = end_p.tv_sec - start_p.tv_sec  + (double)(end_p.tv_nsec - start_p.tv_nsec)/BILLION;
              //  cout << "It took " << difference2 << " seconds to calculate density " << endl;

			clock_gettime(CLOCK_REALTIME, &start_p);

			check_send_lando	= 	send(network_socket, p_lan_do_so, sizeof(int), MSG_NOSIGNAL);
			check_send_density	= 	send(network_socket, p_av_density, sizeof(uint32_t), MSG_NOSIGNAL);
			
			clock_gettime( CLOCK_REALTIME, &end_p);
                double difference3 = end_p.tv_sec - start_p.tv_sec  + (double)(end_p.tv_nsec - start_p.tv_nsec)/BILLION;
               // cout << "It took " << difference3 << " seconds to send final density " << endl;

			while (check_send_lando == -1 || check_send_density == -1 || check_starttime == -1){
				cout << "Disconnect to server!trying to reconnect in 2s ... \n";
				cout << "Previous packet will be dumped! \n";
				///attempt to reconnect/////////////////////////////
				network_socket = socket(AF_INET, SOCK_STREAM, 0);
					
				connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
			
				if(connection_status == -1){
					cout << "Connecting to public ip... " << endl;
					sleep(1);
					
					cout << "Reconnection status: " << connection_status << endl;
				}
				if (connection_status != -1){
					cout << "Reconnect successfully! \n";
					break;
				}
				
   			}
   			cout << "Finish lando number: " << lan_do_so << endl;
			lan_do = 0;
			clock_gettime( CLOCK_REALTIME, &end_p);
			cout << " End time is " << end_p.tv_sec << endl;
			double difference = end_p.tv_sec  + (double)(end_p.tv_nsec)/BILLION - fixedtime;
			cout << "It took " << difference << " seconds to process " << endl;
    	}
    close(network_socket);
    return 0;
}
