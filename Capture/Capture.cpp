
/*** fix 3:43PM---06/11/2018: add buff_vel, memcpy **********/
#include "all_lib.hpp"



#define BILLION 1E9

#define IMG_TEMP "anh_temp.jpeg"

#define ID_CAMERA "5"
#define TIMEOUT 10
#define PORT 9000

using namespace std;

int network_socket;
/*		Su dung cho phan dieu khien Camera 	*/
int fd_cam_main;
struct v4l2_buffer buf_v4l2;
fd_set fds_main;
struct timeval tv_main = {0};
int r;

unsigned char *buffer;
unsigned char buf_vel[921600];	// buf_v4l2.length : 640*480*3 = 921600
int lan_do = 0;
char respon[100] = {0};
int check_send;

int main(int argc, char* argv[])
{
	struct timespec start_p, end_p; //measure time to process
	struct timespec start_d, end_d; //measure time of disconnection
	int sendport=0;
    //Khoi tao socket 
	//char ipaddr[30];
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (network_socket < 0) printf("Error when create socket\n");
	else printf("Successful create socket\n");
	
	cout << "You have entered arguments:" << "\n"; 
  
    for (int i = 0; i < argc; ++i) 
        cout << argv[i] << "\n"; 
  
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr(argv[1]);
	int connection_status=0;
	int tried = 0;
	
	do{
		connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	
		sleep(1);
	}while(connection_status == -1);
	
	//................write image into storage ..........................//
	
	std::ifstream DataFile("abc.jpeg", std::ios::binary);
    if(!DataFile.good())
        return 0;

    DataFile.seekg(0, std::ios::end);
    size_t filesize = (int)DataFile.tellg();
    DataFile.seekg(0);
	std::cout << "Size of image is: " << filesize ;
	DataFile.read((char*)buf_vel, filesize);
	int frame_c = 0;
	
	while(1){
    //......................capture 1 frame...........................//
    	usleep(115000);
		clock_gettime( CLOCK_REALTIME, &start_p);
		double starttime_p, endtime_p;
		double starttime_d = 0; double endtime_d = 0;
		starttime_p = start_p.tv_sec + (double)(start_p.tv_nsec)/BILLION;
		cout << "Prepare to start frame " << frame_c << "Start time is " << start_p.tv_sec << endl;
		send(network_socket, &starttime_p, 8, MSG_NOSIGNAL); // send starting time
		frame_c++;

	//.......................connect to send image.......................//
		//usleep(40000);
		check_send = send(network_socket, buf_vel, 921600, MSG_NOSIGNAL);
		int time_to_out = 0;
		int count = 0;
		while(check_send == -1){
			if(count == 0){
				clock_gettime( CLOCK_REALTIME, &start_d); // get disconnection time
				starttime_d = start_d.tv_sec + (double)(start_d.tv_nsec)/BILLION;
			}
			count ++;
			cout << "Disconnect to server!trying to reconnect in 1s ... \n";
			cout << "Previous packet will be dumped! \n";

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
				cout << "Timeout reached, exit. \n";
				return -1;
			}
		}
		clock_gettime( CLOCK_REALTIME, &end_p);
		double difference_p = endtime_p - starttime_p;
		double difference_d = endtime_d - starttime_d;
		cout << "It took " << difference_p << " seconds to process " << endl;
		cout << "It took " << difference_d << " seconds to reconnect " << endl;
		}
		return 0;
		close(network_socket);
	}
