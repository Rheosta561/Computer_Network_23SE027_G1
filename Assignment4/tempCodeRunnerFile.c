#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define FRAME_SIZE 32
#define MAX_FILE_SIZE 1024
typedef struct
{
    int frame_no;
    int size;
    char data[FRAME_SIZE];
} Frame;
int main()
{
    mkfifo("fifo1", 0666);
    mkfifo("fifo2", 0666);
    int fd1 = open("fifo1", O_WRONLY);
    int fd2 = open("fifo2", O_RDONLY);
    FILE *fp = fopen("input.txt", "r");
    if (!fp)
    {
        perror("File open failed");
        exit(1);
    }
    char buffer[MAX_FILE_SIZE];
    int n = fread(buffer, 1, MAX_FILE_SIZE, fp);
    fclose(fp);
    int total_frames = (n + FRAME_SIZE - 1) / FRAME_SIZE;
    printf("\n[SERVER] Total frames to send: %d\n", total_frames);
    int frame_no = 0;
    int bytes_sent = 0;
    while (bytes_sent < n)
    {
        Frame frame;
        frame.frame_no = frame_no;
        frame.size = (n - bytes_sent > FRAME_SIZE) ? FRAME_SIZE : (n - bytes_sent);
        memcpy(frame.data, buffer + bytes_sent, frame.size);
        write(fd1, &frame, sizeof(Frame));
        printf("[SERVER] Sent Frame %d (Size: %d)\n", frame_no,
               frame.size);
        int ack;
        read(fd2, &ack, sizeof(int));
        if (ack == frame_no)
        {
            printf("[SERVER] Received ACK for Frame %d\n", ack);
            bytes_sent += frame.size;
            frame_no++;
        }
    }
    close(fd1);
    close(fd2);
    unlink("fifo1");
    unlink("fifo2");
    return 0;
}