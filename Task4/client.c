#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_auth_request(char *credentials) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    //Creating Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    //Converting IPv4 address from text to binary format
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        close(sock);
        return;
    }

    //Connecting to the Server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection to server failed! Ensure server is running.\n");
        close(sock);
        return;
    }

    //Send structured string over the network socket pipeline
    printf("[CLIENT] Transmitting payload string: \"%s\"\n", credentials);
    send(sock, credentials, strlen(credentials), 0);

    //Reading the response back from server
    read(sock, buffer, BUFFER_SIZE);
    printf("[SERVER RESPONSE]: %s\n\n", buffer);

    //Close connection
    close(sock);
}

int main() {
    printf("[CLIENT SYSTEM] Starting automated test sequence...\n\n");

    // Scenario 1: Valid credentials for user1
    printf("--- Test Case 1: Testing Valid User1 ---\n");
    send_auth_request("user1:chpasswd");
    sleep(1); // Small delay between network connection simulations

    // Scenario 2: Invalid password attempt
    printf("--- Test Case 2: Testing Incorrect Password ---\n");
    send_auth_request("user2:wrongpassword");
    sleep(1);

    // Scenario 3: Malformed payload data (Missing the colon separation protocol rule)
    printf("--- Test Case 3: Testing Malformed Protocol Data ---\n");
    send_auth_request("hacker_attack_no_colon");
    sleep(1);

    printf("[CLIENT SYSTEM] Testing pipeline complete.\n");
    return 0;
}
