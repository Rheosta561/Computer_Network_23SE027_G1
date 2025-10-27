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
    int fd1 = open("fifo1", O_RDONLY);
    int fd2 = open("fifo2", O_WRONLY);
    char reassembled[MAX_FILE_SIZE];
    int bytes_received = 0;
    while (1)
    {
        Frame frame;
        int r = read(fd1, &frame, sizeof(Frame));
        if (r <= 0)
            break;
        printf("[CLIENT] Received Frame %d (Size: %d)\n",
               frame.frame_no, frame.size);
        memcpy(reassembled + bytes_received, frame.data, frame.size);
        bytes_received += frame.size;
        int ack = frame.frame_no;

        write(fd2, &ack, sizeof(int));
        printf("[CLIENT] Sent ACK for Frame %d\n", ack);
    }
    reassembled[bytes_received] = '\0';
    printf("\n[CLIENT] Reassembled File Content:\n%s\n", reassembled);
    close(fd1);
    close(fd2);
    return 0;
}