#include <sstream>
#include <iostream>
#include "boost/asio.hpp"
#include "grSim_Packet.pb.h"
#include "grSim_Commands.pb.h"

using namespace boost::asio;

std::ostringstream stream;
void send_data(){
    grSim_Packet packet;
    bool yellow = false;
    packet.mutable_commands()->set_isteamyellow(true);
    packet.mutable_commands()->set_timestamp(0.0);
    grSim_Robot_Command* command = packet.mutable_commands()->add_robot_commands();
    command->set_id(0);

    command->set_wheelsspeed(false);
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
    
	packet.SerializeToOstream(&stream);
}


int main(){
	io_service io_service;
	ip::udp::socket socket(io_service);
	ip::udp::endpoint remote_endpoint;
	
	socket.open(ip::udp::v4());
	remote_endpoint = ip::udp::endpoint(ip::address::from_string("127.0.0.1"),20011);
	
	boost::system::error_code err;
	send_data();
	while(true){
		std::string data = stream.str();
		std::cout<<"str: "<<stream.str()<<"\n";	
		socket.send_to(buffer(data,int(data.length())),remote_endpoint,0,err);
	}
	socket.close();
	return 0;
}
