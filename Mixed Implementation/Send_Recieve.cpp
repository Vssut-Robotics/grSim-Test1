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

using namespace std;

class Defense{
	private:
		bool is_defense_mode;
		int32_t robot_x_pos,robot_y_pos,ball_x_pos,ball_y_pos;
	public:
		void KeeperSoloPlay(){}
		void Keeper2DefensePlay(){}
		void Keeper1DefensePlay(){}
}

void printRobotInfo(const SSL_DetectionRobot & robot) {
    printf("CONF=%4.2f ", robot.confidence());
    if (robot.has_robot_id()) {
        printf("ID=%3d ",robot.robot_id());
    } else {
        printf("ID=N/A ");
    }
    printf(" HEIGHT=%6.2f POS=<%9.2f,%9.2f> ",robot.height(),robot.x(),robot.y());
    if (robot.has_orientation()) {
        printf("ANGLE=%6.3f ",robot.orientation());
    } else {
        printf("ANGLE=N/A    ");
    }
    printf("RAW=<%8.2f,%8.2f>\n",robot.pixel_x(),robot.pixel_y());
}

void initialize_send_commands(grSim_Robot_Command* command){
	command->set_id(0);
	command->set_wheelsspeed(true);
	command->set_wheel1(0.0);
	command->set_wheel2(0.0);
	command->set_wheel3(0.0);
	command->set_wheel4(0.0);
	command->set_veltangent(0.0);
	command->set_velnormal(0.0);
	command->set_velangular(0.0);
	command->set_kickspeedx(0.0);
	command->set_kickspeedz(0.0);
	command->set_spinner(false);				
}

void print_detection_data(const SSL_WrapperPacket &packet){
	SSL_DetectionFrame detection = packet.detection();
	//Display the contents of the robot detection results:
	double t_now = GetTimeSec();

	printf("-[Detection Data]-------\n");
	//Frame info:
	printf("Camera ID=%d FRAME=%d T_CAPTURE=%.4f\n",detection.camera_id(),detection.frame_number(),detection.t_capture());

	printf("SSL-Vision Processing Latency                   %7.3fms\n",(detection.t_sent()-detection.t_capture())*1000.0);
	printf("Network Latency (assuming synched system clock) %7.3fms\n",(t_now-detection.t_sent())*1000.0);
	printf("Total Latency   (assuming synched system clock) %7.3fms\n",(t_now-detection.t_capture())*1000.0);
	int balls_n = detection.balls_size();
	int robots_blue_n =  detection.robots_blue_size();
	int robots_yellow_n =  detection.robots_yellow_size();

	//Ball info:
	for (int i = 0; i < balls_n; i++) {
		SSL_DetectionBall ball = detection.balls(i);
		printf("-Ball (%2d/%2d): CONF=%4.2f POS=<%9.2f,%9.2f> ", i+1, balls_n, ball.confidence(),ball.x(),ball.y());
		if (ball.has_z()) {
			printf("Z=%7.2f ",ball.z());
		} else {
			printf("Z=N/A   ");
		}
		printf("RAW=<%8.2f,%8.2f>\n",ball.pixel_x(),ball.pixel_y());
	}

	//Blue robot info:
	for (int i = 0; i < robots_blue_n; i++) {
		SSL_DetectionRobot robot = detection.robots_blue(i);
		printf("-Robot(B) (%2d/%2d): ",i+1, robots_blue_n);
		printRobotInfo(robot);
	}

	//Yellow robot info:
	for (int i = 0; i < robots_yellow_n; i++) {
		SSL_DetectionRobot robot = detection.robots_yellow(i);
		printf("-Robot(Y) (%2d/%2d): ",i+1, robots_yellow_n);
		printRobotInfo(robot);
	}
}

int main(int argc,char* argv[]){
	RoboCupSSLClient client(40102,"224.5.23.2","");
	client.open(true);
	SSL_WrapperPacket recieve_packet;
	//Target 1:
	//Send Bot 5 near the ball
	grSim_Packet send_packet;
	send_packet.mutable_commands()->set_isteamyellow(yellow);
	send_packet.mutable_commands()->set_timestamp(0.0);
	grSim_Robot_Command* command = packet.mutable_commands()->add_robot_commands();
	initialize_send_commands(command);	
	while(true){
		if(client.recieve(recieve_packet)){
            printf("-----Received Wrapper Packet-----\n");			
			if (recieve_packet.has_detection()) {		
				//uncomment if you need to print the detection packet data
				//print_detection_data(recieve_packet);	
				SSL_DetectionFrame detection = recieve_packet.detection();
				SSL_DetectionBall ball = detection.balls(0);
				SSL_DetectionRobot robot = detection.robots_yellow(5);
            }			            
		}
	}
	return 0;
}
