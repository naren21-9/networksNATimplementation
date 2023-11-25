#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void clearInputBuffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

int main() {
  int clientSocket;
  struct sockaddr_in serverAddr;

  // Create socket
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  // Initialize server address struct
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8052);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  // Connect to the server
  connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  int option = 0;

  // Send request to server
  do {
    // Send request to server
    // Menu for user interaction
    printf("\n");
    printf("Menu:\n");
    printf("1. Check IP Mapping\n");
    printf("2. Add New Mapping\n");
    printf("3. Quit\n");

    printf("Enter option: ");
    scanf("%d", &option);
    printf("\n");

    // Clear input buffer to avoid issues with newline character
    clearInputBuffer();

    if (option == 1) {
      send(clientSocket, "1", 1, 0);
      // Option 1: Check and provide corresponding global address
      char localAddress[15];
      printf("Enter local IP address: ");
      scanf("%s", localAddress);

      // Send local address to the server
      send(clientSocket, localAddress, sizeof(localAddress), 0);

      // Receive and print the server's response
      char buffer[1024];
      recv(clientSocket, buffer, sizeof(buffer), 0);
      printf("Response: %s\n", buffer);
    } else if (option == 2) {
      send(clientSocket, "2", 1, 0);
      // Option 2: Add new mapping
      char localAddress[15], globalAddress[15];
      printf("Enter local IP address: ");
      scanf("%s", localAddress);

      // Clear input buffer
      clearInputBuffer();

      printf("Enter global IP address: ");
      scanf("%s", globalAddress);

      // Send local and global addresses to the server
      send(clientSocket, localAddress, sizeof(localAddress), 0);
      send(clientSocket, globalAddress, sizeof(globalAddress), 0);

      // Receive and print the server's response
      char buffer[1024];
      recv(clientSocket, buffer, sizeof(buffer), 0);
      printf("Response: %s\n", buffer);
    }
  } while (option != 3);

  // Close the client socket
  close(clientSocket);

  return 0;
}