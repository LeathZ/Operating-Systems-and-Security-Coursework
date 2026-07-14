#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

//Authentication database same users we created in Task 3
int authenticate_user(char *username, char *password) {
    if (strcmp(username, "user1") == 0 && strcmp(password, "chpasswd") == 0) return 1;
    if (strcmp(username, "user2") == 0 && strcmp(password, "chpasswd") == 0) return 1;
    if (strcmp(username, "user3") == 0 && strcmp(password, "chpasswd") == 0) return 1;
    return 0; //Authentication failed
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    //Creating Socket File Descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //Setting up server address structures
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //Binding the socket to port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //Listening for incoming connections (Queue up to 3 connections)
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Authentication Server running on port %d...\n", PORT);
    printf("[SERVER] Waiting for client connections...\n\n");

    //Loop to handle multiple client connections sequentially
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("Accept connection failed");
            continue; 
        }

        printf("[SERVER] Connected to a new client successfully.\n");
        
        //Clear buffer and read data sent by the client
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        
        //Data validation
        if (valread <= 0 || strchr(buffer, ':') == NULL) {
            char *error_msg = "AUTH_FAILED: Malformed or empty protocol string.";
            send(new_socket, error_msg, strlen(error_msg), 0);
            printf("[SERVER] Error: Invalid protocol format received.\n");
            close(new_socket);
            continue;
        }

        //Protocol parsing
        char *username = strtok(buffer, ":");
        char *password = strtok(NULL, "\n"); // Extract up to the newline character

        printf("[SERVER] Parsing credentials -> User: %s\n", username);

        //Authentication
        if (authenticate_user(username, password)) {
            char *success_msg = "AUTH_SUCCESS: Access Granted.";
            send(new_socket, success_msg, strlen(success_msg), 0);
            printf("[SERVER] Verdict: AUTH_SUCCESS for %s\n", username);
        } else {
            char *fail_msg = "AUTH_FAILED: Invalid Username or Password.";
            send(new_socket, fail_msg, strlen(fail_msg), 0);
            printf("[SERVER] Verdict: AUTH_FAILED\n");
        }

        //Close transaction session with this individual client
        printf("[SERVER] Closing client connection window.\n\n");
        close(new_socket);
    }

    close(server_fd);
    return 0;
}
