#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <fcntl.h>  

int main() {
    int pipe1[2], pipe2[2]; // pipe1:client -- server, pipe2:server -- client
    pid_t pid; // Process ID variable


    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe creation failed"); 
        exit(1);
    }

    // child process
    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // child process = server
        close(pipe1[1]); 
        close(pipe2[0]); 

        int a, b; 
        read(pipe1[0], &a, sizeof(a)); 
        read(pipe1[0], &b, sizeof(b)); 

        int result = a * b; //multiplying two numbers

        write(pipe2[1], &result, sizeof(result)); 

        close(pipe1[0]); 
        close(pipe2[1]); 
        exit(0); // server process exits
    } else {
        // parent process = client
        close(pipe1[0]); 
        close(pipe2[1]); 

        int a, b; 
        printf("Enter two numbers: ");
        scanf("%d %d", &a, &b); // input numbers

        write(pipe1[1], &a, sizeof(a)); 
        write(pipe1[1], &b, sizeof(b)); 

        int result; 
        read(pipe2[0], &result, sizeof(result)); // reading the result from server

        // writing the output to text file 
        FILE *fp = fopen("output.txt", "w");
        if (fp == NULL) {
            perror("file opening failed");
            exit(1);
        }
        fprintf(fp, "Server returned result: %d\n", result);
        fclose(fp);

        printf("Result written to output.txt\n");

        close(pipe1[1]); 
        close(pipe2[0]); 
    }

    return 0; 
}
