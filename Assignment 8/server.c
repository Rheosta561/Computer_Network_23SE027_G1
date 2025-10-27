#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 9020
#define BACKLOG 5
#define BUF_SIZE 1024
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
void handle_client(int newsockfd, struct sockaddr_in cli_addr)
{
    char buffer[BUF_SIZE];
    char *client_ip = inet_ntoa(cli_addr.sin_addr);
    printf("Client connected from %s\n", client_ip);
    FILE *fp = popen("date", "r");
    if (fp == NULL)
    {
        strcpy(buffer, "Error running date command\n");
    }
    else
    {
        fgets(buffer, sizeof(buffer), fp);
        pclose(fp);
    }
    send(newsockfd, buffer, strlen(buffer), 0);
    close(newsockfd);

    printf("Client from %s disconnected\n", client_ip);
}
int main()
{
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error opening socket");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("Error on binding");
    if (listen(sockfd, BACKLOG) < 0)
        error("Error on listen");
    printf("Server listening on port %d...\n", PORT);
    clilen = sizeof(cli_addr);
    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                           &clilen);
        if (newsockfd < 0)
            error("Error on accept");
        pid_t pid = fork();
        if (pid == 0)
        { // Child process
            close(sockfd);
            handle_client(newsockfd, cli_addr);
            exit(0);
        }
        else if (pid > 0)
        {
            close(newsockfd);
        }
        else
        {
            perror("Error forking");
        }
    }

    close(sockfd);
    return 0;
}