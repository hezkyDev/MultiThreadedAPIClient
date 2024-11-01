#include "APIClient.h"
#include <iostream>
#include <string>

int main() {
    APIClient client;
    std::string command;

    std::cout << "Welcome to the Real-Time Weather Fetcher!" << std::endl;
    std::cout << "Commands:\n- 'start' to begin fetching data\n- 'stop' to stop fetching\n- 'view' to view stored data\n- 'exit' to quit the program" << std::endl;

    while (true) {
        std::cout << "\nEnter command: ";
        std::cin >> command;

        if (command == "start") {
            client.startFetching();
        } else if (command == "stop") {
            client.stopFetching();
        } else if (command == "view") {
            client.readWeatherData();
        } else if (command == "exit") {
            client.stopFetching();
            std::cout << "Exiting the program. Goodbye!" << std::endl;
            break;
        } else {
            std::cout << "Unknown command. Please enter 'start', 'stop', 'view', or 'exit'." << std::endl;
        }
    }

    return 0;
}