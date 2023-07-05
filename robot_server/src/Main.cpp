
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Message.pb.h"
#include "Message.pb.cc"

#define PORT 8888

int main(int argc, char const* argv[]) {

    // use these strings to indicate the state transitions
    // the robot progresses through.  Do not modify these strings

    std::string robot_waiting = "The robot is waiting";
    std::string robot_moving = "The robot is moving";

    std::string robot_finished_waiting = "The robot finished waiting";
    std::string robot_finished_moving = "The robot finished moving";

    std::string robot_began_waiting = "The robot began waiting";
    std::string robot_begin_moving = "The robot began moving";

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    bool isMoving = false;
    char buffer[1024] = {0};
    int tick_counter = 0;

    while (true) {
        read(new_socket, buffer, 1024);
        small_world::SM_Event event;
        event.ParseFromString(buffer);

        if (event.event_type() == "tick") {
            tick_counter++;
            
            if (isMoving) {
                if (tick_counter % 2 == 0) {
                    std::cout << robot_finished_moving << std::endl;
                }
                else {
                    std::cout << robot_moving << std::endl;
                }
            }
            else {
                if (tick_counter % 2 == 0) {
                    std::cout << robot_finished_waiting << std::endl;
                } else {
                    std::cout << robot_waiting << std::endl;
                }
            }
            
            if (tick_counter % 2 == 0) {
                std::cout << (isMoving ? robot_began_waiting : robot_begin_moving) << std::endl;
                isMoving = !isMoving;
            }
        }
    }

    return EXIT_SUCCESS;
}
