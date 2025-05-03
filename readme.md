# To run the code

Save the server code as ``udp_echo_server`.c` (or .cpp) and client code as `udp_echo_client.c` (or .cpp).
Open a terminal.
Compile the server: g++ `udp_echo_server.c` -o `udp_echo_server`
Compile the client: g++ `udp_echo_client.c` -o `udp_echo_client`
Run the server first: `./udp_echo_server`
Run the client in another terminal: `./udp_echo_client`
For local testing, SERVER_IP in the client code should be 127.0.0.1 (localhost).
