#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cstdio> // For perror

#define PORT 12345 // The port clients will connect to
#define MAX_BUFFER_SIZE 1024


#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cstdio> // For perror

#define PORT 12345 // The port clients will connect to
#define MAX_BUFFER_SIZE 1024

int main() {
    int sockfd;
    char buffer[MAX_BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    ssize_t recv_len;

    // 1. Create UDP socket
    // AF_INET: IPv4 Internet protocols
    // SOCK_DGRAM: Datagram socket (UDP)
    // 0: Default protocol for the type
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    // 2. Prepare the server address structure
    memset(&server_addr, 0, sizeof(server_addr)); // Clear the structure
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(PORT); // Port number, convert to network byte order
    // server_addr.sin_addr.s_addr = inet_addr("192.168.1.100"); // Bind to a specific IP
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface

    // 3. Bind the socket to the server address and port
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        close(sockfd);
        return 1;
    }

    std::cout << "UDP Echo Server listening on port " << PORT << "..." << std::endl;

    // 4. Main loop to receive and send data
    while (true) {
        client_addr_len = sizeof(client_addr); // Initialize client address length

        // Receive data from a client
        // recvfrom will fill client_addr with the sender's address and port
        recv_len = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0,
                            (struct sockaddr *)&client_addr, &client_addr_len);

        if (recv_len < 0) {
            perror("Error receiving data");
            // In a real server, you might continue the loop or handle specific errors
            continue; // Keep listening for the next packet
        }

        // Null-terminate the received data to treat it as a string (optional, depends on data type)
        // Make sure not to write past the buffer size
        if (recv_len < MAX_BUFFER_SIZE) {
             buffer[recv_len] = '\0';
        } else {
             buffer[MAX_BUFFER_SIZE - 1] = '\0'; // Truncate if needed
        }


        // Get client IP address for logging
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

        std::cout << "Received message from " << client_ip << ":" << ntohs(client_addr.sin_port)
                  << ": " << buffer << std::endl;

        // Send the received data back to the client
        // sendto uses the address information obtained from recvfrom
        ssize_t send_len = sendto(sockfd, buffer, recv_len, 0,
                                  (const struct sockaddr *)&client_addr, client_addr_len);

        if (send_len != recv_len) {
            // This is not necessarily an error in UDP, but indicates not all bytes were sent
            // UDP doesn't guarantee delivery
            perror("Warning: Sent different number of bytes than received");
        }
    }

    // 5. Close the socket (This part is unreachable in the infinite loop)
    close(sockfd);

    return 0;
}
