#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_TEXT 200
#define FLAG '~'
#define ESC  '}'

struct my_msgbuf {
    long mtype;
    char mtext[MAX_TEXT];
};

// function for character count framing
void send_character_count(int msgid, char *buffer, int len) {
    struct my_msgbuf msg;
    msg.mtype = 1;


    msg.mtext[0] = (char)(len + 1); 
    memcpy(msg.mtext + 1, buffer, len);

    msgsnd(msgid, &msg, len + 1, 0);
    printf("[Sender] Sent frame (Character Count): %s\n", msg.mtext + 1);
}

void receive_character_count(int msgid, FILE *out) {
    struct my_msgbuf msg;
    msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);

    int count = msg.mtext[0];
    fprintf(out, "Received CC Frame (len=%d): ", count);

    for (int i = 1; i < count; i++) {
        fputc(msg.mtext[i], out);
    }
    fputc('\n', out);
}

// function for byte stuffing framing
void send_byte_stuffing(int msgid, char *buffer, int len) {
    struct my_msgbuf msg;
    msg.mtype = 2;

    int j = 0;
    msg.mtext[j++] = FLAG; // starting flag

    for (int i = 0; i < len; i++) {
        if (buffer[i] == FLAG || buffer[i] == ESC) {
            msg.mtext[j++] = ESC; // stuff
        }
        msg.mtext[j++] = buffer[i];
    }

    msg.mtext[j++] = FLAG; // ending flag
    msgsnd(msgid, &msg, j, 0);
    printf("[Sender] Sent frame (Byte Stuffing).\n");
}

void receive_byte_stuffing(int msgid, FILE *out) {
    struct my_msgbuf msg;
    int r = msgrcv(msgid, &msg, sizeof(msg.mtext), 2, 0);

    fprintf(out, "Received BS Frame: ");
    for (int i = 1; i < r - 1; i++) { 
        if (msg.mtext[i] == ESC) {
            i++; 
        }
        fputc(msg.mtext[i], out);
    }
    fputc('\n', out);
}

int main() {
    key_t key;
    int msgid;
    FILE *in, *out;
    char buffer[100];

    // generate key & create message queue
    key = ftok("progfile", 65);
    msgid = msgget(key, 0666 | IPC_CREAT);

    in = fopen("input.txt", "r");
    if (!in) {
        perror("input.txt missing");
        exit(1);
    }

    out = fopen("output.txt", "w");
    if (!out) {
        perror("output.txt open failed");
        exit(1);
    }

    if (fork() == 0) {
        //  sender 
        while (fgets(buffer, sizeof(buffer), in)) {
            int len = strlen(buffer);
            if (buffer[len - 1] == '\n') buffer[len - 1] = '\0', len--;

            send_character_count(msgid, buffer, len);
            send_byte_stuffing(msgid, buffer, len);
            sleep(1);
        }
        exit(0);
    } else {
        // receiver 
        for (int i = 0; i < 2; i++) { //  2 msgs for each line
            receive_character_count(msgid, out);
            receive_byte_stuffing(msgid, out);
        }
        wait(NULL);
    }

    fclose(in);
    fclose(out);
    msgctl(msgid, IPC_RMID, NULL); // destroy queue
    return 0;
}
