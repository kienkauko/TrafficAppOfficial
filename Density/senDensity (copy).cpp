#include "all_lib.hpp"
#include "density.hpp"
#include "send.hpp"
#include "receive.hpp"

#define SO_LAN_DO 10

int av_density_arr[SO_LAN_DO];
int lan_do = 0;

float32_t muy[307200], var[307200];
uint8_t bin[307200]; 
uint8_t confirm_bg = 0;
uint8_t *pconfirm_bg = &confirm_bg;
uint32_t av_density;
uint32_t *p_av_density = &av_density;
int frame_count = 0;

struct timeval tv_main = {0};

void density(){
	av_density = findDensity_test(gray_a[0], muy, var, bin);

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
		
		//khoi tao socket
	  int socket_desc , new_socket , c; //receive data
	  int network_socket;		// send data
      struct sockaddr_in server , client;
	  struct timespec start_p, end_p; //measure time to process
      //Create socket
      socket_desc = socket(AF_INET , SOCK_STREAM , 0);
      if (socket_desc == -1)
      {
         printf("Could not create socket");
      }
	 // Forcefully attaching socket to port
	 int opt = 1;
      if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt");
        exit(EXIT_FAILURE);
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
     while(listen(socket_desc , 3) != -1){

      //Accept and incoming connection
      //puts("Waiting for incoming connections...");
      c = sizeof(struct sockaddr_in);

     

    fflush(stdout);
    
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (network_socket < 0) printf("Error when create socket\n");
	else printf("Successful create socket\n");
	// specify an address for the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9007);
	//server_address.sin_addr.s_addr = inet_addr(ip);
	int connection_status = 0;
	int reconnection_flag=0;
	char ipaddress[50];
	do{
		server_address.sin_addr.s_addr = inet_addr("172.18.0.3");
		connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
		if(connection_status == -1){
			
			cout << "Fail to connect to localhost." << endl;
			cout << "Please enter public IP: " << endl;
			fgets( ipaddress, 50, stdin );
			server_address.sin_addr.s_addr = inet_addr(ipaddress);
			connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
			reconnection_flag=1;
		}
	}while(connection_status == -1);

    /****************************************************************/
    ////nhan lien tuc
	if((new_socket = accept(socket_desc, (struct sockaddr *)&server,(socklen_t*)&c))){
		puts("Connection accepted");
		//receiveInt(new_socket, pconfirm_bg);
		//cout << "Receiving status: " << int(confirm_bg) << endl;
		//if (confirm_bg == 0){
    		receiveFloat(new_socket, muy);
    		receiveFloat(new_socket, var);
    	//}
    	cout << "Done background initiation \n";
    	
    	while(1){
    		new_socket = accept(socket_desc, (struct sockaddr *)&server,(socklen_t*)&c);
    		receiveInt(new_socket, pconfirm_bg);
    		cout << "Receiving status: " << int(confirm_bg) << endl;
    		if (confirm_bg == 1){			//send gray image
    			if (lan_do == 0){
    				clock_gettime( CLOCK_REALTIME, &start_p); // for cal time
    			}
    			
    			receive2D(new_socket);	//nhan image in  gray
    			
    			density();		//chay ham density
    			av_density_arr[lan_do] = av_density;
    			++lan_do;
				if (lan_do < SO_LAN_DO ) continue;
				
				// Neu chua do du so lan thi chua gui anh ve server
				av_density = 0;
				for (int i = 0; i < SO_LAN_DO; ++i)
				{
					av_density += av_density_arr[i];
				}
				av_density = av_density / SO_LAN_DO;
				clock_gettime( CLOCK_REALTIME, &end_p);
				
				lan_do_so++;
				cout << "Lan do: " << lan_do_so << endl;
				cout << "Final density: " << av_density << "%" << endl;
				
				
				// calculate time-taken
				difference = (end_p.tv_sec - start_p.tv_sec) + (float)
				(end_p.tv_nsec - start_p.tv_nsec)/1000000000.0d;
				cout << "It took " << difference << " seconds to process " << endl;
				
				//connect to send lan_do_so, av_density, time process
				
				network_socket = socket(AF_INET, SOCK_STREAM, 0);
				if(reconnection_flag !=1 ){
				connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
				}
				reconnection_flag = 0;
				while (connection_status == -1){
					cout << "Coundn't connect to server Density \n";
					cout << "Trying to reconnect in 1s... \n";
					sleep(1);
					connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
					
				}
				//uint8_t(av_density);
				sendInt(network_socket, p_lan_do_so, 4);
				sendUint32(network_socket, p_av_density, 4);
				sendTime(network_socket, p_difference, sizeof(float));
				
				close(network_socket);
				
				lan_do = 0;
    		}
    		else{
    			cout << "Client restarted, the following packets will be dumped: muy, var \n";
    			receiveFloat(new_socket, muy);
    			receiveFloat(new_socket, var);
    		}

    	}
    }
    }
}

