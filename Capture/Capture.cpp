
/*** fix 3:43PM---06/11/2018: add buff_vel, memcpy **********/
#include "all_lib.hpp"



#define BILLION 1E9

#define IMG_TEMP "anh_temp.jpeg"

#define ID_CAMERA "5"
#define TIMEOUT 30

using namespace std;
//using namespace cv;

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

    //Khoi tao socket 
	//char ipaddr[30];
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (network_socket < 0) printf("Error when create socket\n");
	else printf("Successful create socket\n");
	
	
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	int connection_status=0;
	int tried = 0;
	
	do{
		connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	
		sleep(1);
	}while(connection_status == -1);
	
	
	//----------------------------- Cam Init ------------------------------
	// Open a descriptor to the device

    if ((fd_cam_main = open("/dev/video0", O_RDWR)) < 0){
		perror("open");
		exit(1);
	}
	else
		cout << "\n Open _dev_video0: OK";

	// Retrieve the device’s capabilities
	struct v4l2_capability cap;
    if (ioctl(fd_cam_main, VIDIOC_QUERYCAP, &cap) < 0) {
		perror("VIDIOC_QUERYCAP");
		exit(1);
    }
    else
    	cout << "\n VIDIOC_QUERYCAP: Ok";

	if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
		fprintf(stderr, "The device does not handle single-planar video capture.\n");
		exit(1);
	}
	else
		cout << "\nThe device does not handle single-planar video capture.";

	// Set our video format
	struct v4l2_format fmt;
	fmt.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	//fmt.fmt.pix.field = V4L2_FIELD_ANY;
	fmt.fmt.pix.width       = 640;
    fmt.fmt.pix.height      = 480;

	if(ioctl(fd_cam_main, VIDIOC_S_FMT, &fmt) < 0){
		perror("VIDIOC_S_FMT");
		exit(1);
	}
	else
		cout << "\nVIDIOC_S_FMT: OK";

	// Inform the device about your future buffers
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == ioctl(fd_cam_main, VIDIOC_REQBUFS, &req))
    {
        perror("Requesting Buffer");
        return 1;
    }
    else
    	cout << "\n Requesting Buffer: OK";

	// Allocate your buffers
	//tuyencmt
	//struct v4l2_buffer buf_v4l2;

	memset(&buf_v4l2, 0, sizeof(buf_v4l2));
    buf_v4l2.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf_v4l2.memory = V4L2_MEMORY_MMAP;
    buf_v4l2.index = 0;

    if(ioctl(fd_cam_main, VIDIOC_QUERYBUF, &buf_v4l2) < 0){
        perror("VIDIOC_QUERYBUF");
        exit(1);
    }
    
// for decode function
	buffer = (unsigned char*)mmap(NULL, buf_v4l2.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_cam_main, buf_v4l2.m.offset);

    // buf_v4l2.length : 640*480*3 = 921600
    if(buffer == MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    else
    	cout << "\nmmap: OK";
    //memset(buffer, 0, buf_v4l2.length);

    if (-1 == ioctl(fd_cam_main, VIDIOC_STREAMON, &buf_v4l2.type)) {
        perror("Start Capture");
        return 1;
    }
    else
    	cout << "\n Starting Capture...\n";
	// -------------------------- Discard first frames ------------------------
	for(int i=0; i<10; i++){
		if (-1 == ioctl(fd_cam_main, VIDIOC_QBUF, &buf_v4l2)) {
            perror("Query Buffer");
            return 1;
        }

		FD_ZERO(&fds_main);
		FD_SET(fd_cam_main, &fds_main);
		tv_main.tv_sec = 2;
		r = select(fd_cam_main + 1, &fds_main, NULL, NULL, &tv_main);
		if (-1 == r) {
			perror("Waiting for Frame");
			return 1;
		}

        if (-1 == ioctl(fd_cam_main, VIDIOC_DQBUF, &buf_v4l2)) {
            perror("Retrieving Frame");
            return 1;
        }
	}
	
	/*----------------------------------------------------------------*/
	/*---------------End TESTING CAMERA-------------------------------*/
	
	int frame_c = 0;
	
	/*-----------------------------------------------------------------*/
	/*----------------------Bat dau vong while ------------------------*/
	while(1){
    //......................capture 1 frame...........................//
    	usleep(115000);
		clock_gettime( CLOCK_REALTIME, &start_p);
		double starttime = start_p.tv_sec + (double)(start_p.tv_nsec)/BILLION;
		cout << "Prepare to start frame " << frame_c << "Start time is " << start_p.tv_sec << endl;
		send(network_socket, &starttime, 8, MSG_NOSIGNAL); // send starting time
		frame_c++;
		if (-1 == ioctl(fd_cam_main, VIDIOC_QBUF, &buf_v4l2)) {
            perror("Query Buffer");
            return 1;
        }

		FD_ZERO(&fds_main);
		FD_SET(fd_cam_main, &fds_main);
		tv_main.tv_sec = 2;
		r = select(fd_cam_main + 1, &fds_main, NULL, NULL, &tv_main);
		if (-1 == r) {
			perror("Waiting for Frame");
			return 1;
		}

        if (-1 == ioctl(fd_cam_main, VIDIOC_DQBUF, &buf_v4l2)) {
            perror("Retrieving Frame");
            return 1;
        }

        memcpy(buf_vel, buffer, buf_v4l2.length); 

	
	clock_gettime( CLOCK_REALTIME, &end_p);
                double difference1 = end_p.tv_sec - start_p.tv_sec  + (double)(end_p.tv_nsec - start_p.tv_nsec)/BILLION;
                cout << "It took " << difference1 << " seconds to taake a picture " << endl;
	//................write image into storage ..........................//
	
	
	//.......................connect to send image.......................//
		//usleep(40000);
		clock_gettime(CLOCK_REALTIME, &start_p);
		check_send = send(network_socket, buf_vel, 921600, MSG_NOSIGNAL);
		clock_gettime( CLOCK_REALTIME, &end_p);
                double difference2 = end_p.tv_sec - start_p.tv_sec  + (double)(end_p.tv_nsec - start_p.tv_nsec)/BILLION;
                cout << "It took " << difference2 << " seconds to send image " << endl;
		while(check_send == -1){
		
			cout << "Disconnect to server!trying to reconnect in 2s ... \n";
			cout << "Previous packet will be dumped! \n";
			///attempt to reconnect/////////////////////////////
			network_socket = socket(AF_INET, SOCK_STREAM, 0);
			
			//server_address.sin_addr.s_addr = inet_addr(argv[1]);
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
	clock_gettime(CLOCK_REALTIME, &end_p);
	double difference = end_p.tv_sec  + (double)(end_p.tv_nsec)/BILLION - starttime;
	cout << "It took " << difference << " seconds to process " << endl;
	}
	return 0;
	close(network_socket);
}
