#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Define a structure to represent the mapping
typedef struct {
  char localAddress[15];
  char globalAddress[15];
} Mapping;

// Function to check if the given address is valid
int isValidAddress(const char *address) {
  struct sockaddr_in sa;
  return inet_pton(AF_INET, address, &(sa.sin_addr)) != 0;
}

// Function to handle client requests
void handleRequest(int clientSocket, Mapping mappings[], int *numMappings) {
  // Receive the request from the client

  char buffer[1024];
  if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0) {
    perror("Error receiving option from client");
    return;
  }
  printf("\n");
  int option = atoi(buffer);
  printf("Received option: %d\n\n", option);

  if (option == 1) {
    // Option 1: Check and provide corresponding global address
    char localAddress[15];
    if (recv(clientSocket, localAddress, sizeof(localAddress), 0) < 0) {
      perror("Error receiving local address from client");
      return;
    }
    // printf("nummapping %d\n ", *numMappings);
    printf("Received local address: %s\n", localAddress);

    // if (isValidAddress(localAddress)) {
    int i;
    int match_found = 0; // Flag to check if a match is found

    for (i = 0; i < 2; i++) {
      // printf(

      if (strncmp(mappings[i].localAddress, localAddress,
                  strlen(localAddress)) == 0) {
        printf("Local Address found, Global Address sent to client!\n");
        if (send(clientSocket, mappings[i].globalAddress,
                 sizeof(mappings[i].globalAddress), 0) < 0) {
          perror("Error sending global address to client");
        }
        break;
      }
    }
    if (i == *numMappings) {
      if (send(clientSocket, "Address not found", sizeof("Address not found"),
               0) < 0) {
        perror("Error sending response to client");
      }
    }
    //}
    /*else
    {
      if (send(clientSocket, "Invalid address format",
               sizeof("Invalid address format"), 0) < 0) {
        perror("Error sending response to client");
      }
    }*/
  } else if (option == 2) {
    // Option 2: Add new mapping
    Mapping newMapping;
    if (recv(clientSocket, newMapping.localAddress,
             sizeof(newMapping.localAddress), 0) < 0) {
      perror("Error receiving local address from client");
      return;
    }
    if (recv(clientSocket, newMapping.globalAddress,
             sizeof(newMapping.globalAddress), 0) < 0) {
      perror("Error receiving global address from client");
      return;
    }

    printf("Received local address: %s\n", newMapping.localAddress);
    printf("Received global address: %s\n", newMapping.globalAddress);

    if (isValidAddress(newMapping.localAddress) &&
        isValidAddress(newMapping.globalAddress)) {
      mappings[*numMappings] = newMapping;
      (*numMappings)++;
      if (send(clientSocket, "Mapping added successfully",
               sizeof("Mapping added successfully"), 0) < 0) {
        perror("Error sending response to client");
      }
    } else {
      if (send(clientSocket, "Invalid address format",
               sizeof("Invalid address format"), 0) < 0) {
        perror("Error sending response to client");
      }
    }
  }
}

int main() {
  int serverSocket, clientSocket;
  struct sockaddr_in serverAddr, clientAddr;
  socklen_t addrSize = sizeof(struct sockaddr_in);

  // Define some initial mappings (you can add more)
  Mapping mappings[10];
  int numMappings = 2;

  strcpy(mappings[0].localAddress, "192.168.1.2");
  strcpy(mappings[0].globalAddress, "203.0.113.1");
  strcpy(mappings[1].localAddress, "1.1.1.1");
  strcpy(mappings[1].globalAddress, "2.2.2.2");
  // Create socket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);

  // Initialize server address struct
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8000);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  // Bind the socket to the specified port
  if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
      0) {
    perror("Error binding server socket");
    return 1;
  }

  // Listen for incoming connections
  if (listen(serverSocket, 5) < 0) {
    perror("Error listening for connections");
    return 1;
  }

  printf("Server listening on port 8000...\n");

  while (1) {

    // Accept a connection from the client
    clientSocket =
        accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
    // printf("there");
    //  Handle the client request
    handleRequest(clientSocket, mappings, &numMappings);

    // Close the client socket
    close(clientSocket);
    exit(0);
  }

  // Close the server socket
  close(serverSocket);

  return 0;
}