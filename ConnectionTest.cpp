#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include "grSim_Commands.pb.h"
#include "grSim_Packet.pb.h"
#define PORT 20011
using namespace std;
struct sockaddr_in address;
int sock = 0,valread;
struct sockaddr_in serv_addr;
void send_data(){
    grSim_Packet packet;
    bool yellow = false;
    packet.mutable_commands()->set_isteamyellow(true);
    packet.mutable_commands()->set_timestamp(0.0);
    grSim_Robot_Command* command = packet.mutable_commands()->add_robot_commands();
    command->set_id(0);

    command->set_wheelsspeed(true);
    command->set_wheel1(0.0);
    command->set_wheel2(0.0);
    command->set_wheel3(0.0);
    command->set_wheel4(0.0);
    command->set_veltangent(10.0);
    command->set_velnormal(0.0);
    command->set_velangular(0.0);

    command->set_kickspeedx(0.0);
    command->set_kickspeedz(0.0);
    command->set_spinner(false);
    
	std::ostringstream stream;
	packet.SerializeToOstream(&stream);
	std::string ooop = stream.str();
//	packet.SerializeToString(&ooop);
	std::string data = stream.str();
	std::cout<<"str: "<<stream.str()<<"\n";
	//Send the Command
	send(sock , data.c_str() , strlen(data.c_str()) , 0 );
	//Command Sent
	printf("Data Sent.\n");
}

int main(){
	char *hello = "Hello From Client";
	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
//    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
  //  {
  //      printf("\nInvalid address/ Address not supported \n");
  //      return -1;
  //  }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    int x = 0;
	while(x<1000){
    printf("Sending\n");
    send_data();}
    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    //valread = read( sock , buffer, 1024);
    //printf("%s\n",buffer );
    return 0;
}
