#include <sys/socket.h> // for socket functions
#include <string.h> // for string manipulation
#include <fcntl.h> // for file operations (open)
#include <sys/sendfile.h>
#include <unistd.h> // for close function
#include <netinet/in.h> // for sockaddr_in structure

/*
What does this code do?
This is a basic implementation of an HTTP server:
   - It listens on port 8080.
   - Accepts incoming TCP connections.
   - Reads a client request.
   - Extracts the filename from the HTTP request.
   - Sends the content of the file back to the client.
 */


/*
 * Sockets
 * A socket is an endpoint for communication between two machines. It acts like a telephone connection.
 */

void main() {
	/*
	 * int socket(int domain,int type,int protocol)
	 *  AF_INET -> IPv4 Internet protocols
	 *  SOCK_STREAM -> TCP, provides sequenced, reliable, two-way, connection-based byte streams.
	 *  0 -> default protocol
	*/

	int s = socket(AF_INET,SOCK_STREAM,0);

	// addr -> Represents an IPv4 address.
	// reverse hex of 8080 -> 0x901f, 
	// internet domain sockets
	// better to use htons() for the port number
	struct sockaddr_in addr = {
		AF_INET, // family
		// 0x901f, // port number -> 8080 - (use htons(8080) in real scenarios)
		htons(8080),
		0, //ipv4 address -> 0 means all
	};

	/*
	 * int bind(int sockfd, sockaddr, socklen)
	*/
	bind(s,&addr,sizeof(addr));

	// Initializes a socket for incoming connections
	// listen(int socket,int backlog)
	//   backlog -> connection waiting at the same time
	//   the 10 indicates the maximum number of pending connections in the queue.
	listen(s,10);

	// accept(socket, address, addrlen)
	// client file descriptor
	int client_fd = accept(s,0,0);

	char buffer[256] = {0};
	// recv -> receive the string that client is sending
	// 256 -> len
	recv(client_fd,buffer,256,0);

	// Example of typical HTTP request -> GET /file.html HTTP/1.1
	// Only get the name of the file
	char* f = buffer + 5; // escape 5 bytes to get to the file name. Skip "GET /"
	*strchr(f,' ') = 0; // change the space to null terminator, which leaves only the name of the file (file.html)

	// open the file in readonly mode
	int opened_fd = open(f,O_RDONLY);

	// transfer data from the file to the client socket
	// sendfile(int output_file_descriptor, int input_fd,int offset,int count)
	sendfile(client_fd,opened_fd,0,256);

	// close() comes from unistd
	close(opened_fd);
	close(client_fd);
	close(s);
}