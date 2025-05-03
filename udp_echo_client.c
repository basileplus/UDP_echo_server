#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cstdio> // For perror
#include <limits> // For cin.ignore

#define SERVER_IP "127.0.0.1" // Change this to the server's public IP or internal IP for local testing
#define PORT 12345 // The server's port
#define MAX_BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in server_addr;
    ssize_t recv_len;
    std::string message;

    // 1. Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    // 2. Prepare the server address structure
    memset(&server_addr, 0, sizeof(server_addr)); // Clear the structure
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(PORT); // Server's port, convert to network byte order

    // Convert server IP string to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        return 1;
    }

    std::cout << "UDP Echo Client. Type messages to send (type 'quit' to exit)." << std::endl;

    // 3. Main loop to send and receive data
    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);

        if (message == "quit") {
            break; // Exit loop
        }

        // Clear the input buffer in case of errors or leftover characters
        // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


        // Send data to the server
        // No need to bind client socket; OS assigns a temporary port
        ssize_t send_len = sendto(sockfd, message.c_str(), message.length(), 0,
                                  (const struct sockaddr *)&server_addr, sizeof(server_addr));

        if (send_len < 0) {
            perror("Error sending data");
            continue; // Try again
        }

        std::cout << "Sent " << send_len << " bytes." << std::endl;

        // Receive the echoed data back from the server
        // The server's address will be filled into a temporary sockaddr_in struct if needed,
        // but for echo we expect it from the server we sent to.
        // We still need a sockaddr_in structure and its length pointer for recvfrom's signature.
        struct sockaddr_in temp_addr;
        socklen_t temp_addr_len = sizeof(temp_addr);

        // Note: recvfrom can block indefinitely. In a real application, you might set a timeout
        // using setsockopt (SO_RCVTIMEO).
        recv_len = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0,
                            (struct sockaddr *)&temp_addr, &temp_addr_len);

        if (recv_len < 0) {
            perror("Error receiving data");
            // If using timeouts, check errno for EAGAIN or EWOULDBLOCK
            continue; // Try sending again
        }

        buffer[recv_len] = '\0'; // Null-terminate the received data

        // Optional: Verify the response came from the expected server IP/port
        // This is good practice in UDP if you might receive packets from elsewhere
        // if (temp_addr.sin_addr.s_addr != server_addr.sin_addr.s_addr ||
        //     temp_addr.sin_port != server_addr.sin_port) {
        //     std::cerr << "Warning: Received response from unexpected source!" << std::endl;
        // }


        std::cout << "Received echo: " << buffer << std::endl;
    }

    // 4. Close the socket
    close(sockfd);

    std::cout << "Client shutting down." << std::endl;

    return 0;
}
