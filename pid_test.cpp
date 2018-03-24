#include <cstring>
#include <cstdlib>
#include <iostream>
#include "timer.h"
#include "grSim_Packet.pb.h"
#include "grSim_Commands.pb.h"
#include "robocup_ssl_client.h"
#include "grSim_Replacement.pb.h"
#include "messages_robocup_ssl_wrapper.pb.h"
#include "messages_robocup_ssl_geometry.pb.h"
#include "messages_robocup_ssl_detection.pb.h"

#define PORT 20011

using namespace std;

class pid{
	private:
		float kp,ki,kd;
		double error,last_error,integral,derivative,min_limit,max_limit,val_pid;
	public:
		pid(float _kp,float _ki,float _kd,double _min_limit,double _max_limit){
			kp = _kp;
			ki = _ki;
			kd = _kd;
			min_limit = _min_limit;
			max_limit = _max_limit;
			last_error = 0.0;
		}
		double calculate(double set_point,double current_reading){
			error = current_reading - set_point;
			integral += error;
			if(integral < min_limit) integral = min_limit;
			if(integral > max_limit) integral = max_limit;
			derivative = error - last_error;
			last_error = error;
			val_pid = kp * error + ki * integral + kd * derivative;
			return val_pid;
		}
};

//Overload == operator for comparision of robot positions
bool operator==(const SSL_DetectionRobot& p1, const SSL_DetectionRobot& p2){
	if(p1.x() == p2.x() && p1.y() == p2.y())
		return true;
	return false;
}

int main(int argc,char** argv){
    
    RoboCupSSLClient client(40102,"224.5.23.2","");
    SSL_WrapperPacket recieve_packet;
    
    //Starting Connection to Command Server
    printf("Connecting to Command Server........\n");
	int sock = 0,valread;
	struct sockaddr_in serv_addr;
	string hello = "Hello From Client",buffer;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("Socket creation error\n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed\n");
        return -1;
    }
    send(sock , hello.c_str() , strlen(hello.c_str()) , 0 );
    printf("Connected Successfully to Command Server.\n");
    //Server Correctly Connected
    
    //Starting Connection to grSim Multicast Server
    printf("Connecting to Multicast Server......\n");
    client.open(true);
	if (!client.receive(recieve_packet)){
		printf("Connection to Server Unsuccessful!!\n");
		return -1;
	}
	printf("Client Successfully Connected\n");
	if (!recieve_packet.has_detection()){
		printf("Recieved Packet has no Detection Frame!!\n");
		return -1;
	}
	printf("Everything's Going well buddy!!\n");
	//Server Correctly Connected
	
	//Start fetching data from the detected packet
	SSL_DetectionFrame detection = recieve_packet.detection();
	SSL_DetectionBall ball = detection.balls(0);
	SSL_DetectionRobot tracking_robot_pos,destination_robot_position;
	
	float kp_test,ki_test,kd_test,min_test,max_test;
	destination_robot_position.set_x(ball.x());
	destination_robot_position.set_y(ball.y());
	
	bool reached = false;
	while(!reached){
		tracking_robot_pos = detection.robots_yellow(1);//Send robot-1 near the ball
		printf("Current Robot Position: (%9.2f , %9.2f)",tracking_robot_pos.x(),tracking_robot_pos.y());
		if(tracking_robot_pos == destination_robot_position) reached = true;
		grSim_Packet send_packet;
		send_packet.mutable_commands()->set_isteamyellow(true);
		send_packet.mutable_commands()->set_timestamp(0.0);
		grSim_Robot_Command* command = send_packet.mutable_commands()->add_robot_commands();
		command->set_id(1);

		command->set_wheelsspeed(false); //ambiguous
		command->set_wheel1(0.0);
		command->set_wheel2(0.0);
		command->set_wheel3(0.0);
		command->set_wheel4(0.0);
		command->set_veltangent(3.0); //change
		command->set_velnormal(0.0);
		command->set_velangular(0.0);

		command->set_kickspeedx(1.0);
		command->set_kickspeedz(0.0);
		command->set_spinner(false);		
		
		string data;
		send_packet.SerializeToString(&data);
		//Send the Command
		send(sock , data.c_str() , strlen(data.c_str()) , 0 );
		//Command Sent
		printf("Data Sent.\n");
	}
	return 0;	
}
