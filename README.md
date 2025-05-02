# Real-Time Chat Application in C

This is a simple real-time chat application built using TCP sockets in C. It allows multiple clients to connect to a server and exchange messages in real time.

## Features
- Multi-client support using threads.
- Real-time message broadcasting to all connected clients.
- Each client is identified by a unique name.
- Timestamped messages for better readability.

## How It Works
1. The **server** listens for incoming connections on a specified port.
2. Clients connect to the server and send their name as an identifier.
3. Messages sent by any client are broadcast to all other connected clients.

## Requirements
- GCC or any C compiler.
- Linux environment (or any system supporting POSIX sockets).
- Basic knowledge of terminal commands.

## How to Run
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd <repository-folder>
   ```

2. Compile the server and client:
   ```bash
   gcc -pthread server.c -o server
   gcc -pthread client.c -o client
   ```

3. Start the server:
   ```bash
   ./server
   ```

4. Start one or more clients in separate terminals:
   ```bash
   ./client
   ```

5. Enter your name in the client terminal and start chatting!

## Example
- **Server Output**:
  ```
  Server started on port 8888
  New client connected
  Client Alice connected
  Broadcasting: [12:34:56] Alice: Hello, everyone!
  ```

- **Client Output**:
  ```
  Enter your name: Alice
  Connected to server
  [12:34:56] Bob: Hi Alice!
  You: Hello, everyone!
  ```

## Acknowledgment
This project was developed with guidance from the YouTube video:  
[**"C Programming: Real-Time Chat Application"**](https://www.youtube.com/watch?v=bvyGY1UHJ9o).  
Special thanks to the creator for the tutorial and inspiration.

## Notes
- The server supports up to 20 simultaneous clients by default (can be adjusted in the code).
- The application uses `pthread` for handling multiple clients concurrently.

## License
This project is based on the concepts and code from the referenced YouTube video. Please refer to the video's description or contact the creator for licensing details.
