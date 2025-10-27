#include <stdio.h>
#include <stdlib.h>

void find_class_and_network(char ip[])
{
    int p1, p2, p3, p4;
    sscanf(ip, "%d.%d.%d.%d", &p1, &p2, &p3, &p4);

    printf("IP Address: %s\n", ip);

    if (p1 >= 1 && p1 <= 126) {
        printf("Class A\n");
        printf("Network ID: %d.0.0.0\n", p1);
        printf("Host ID: %d.%d.%d\n", p2, p3, p4);
    }
    else if (p1 >= 128 && p1 <= 191) {
        printf("Class B\n");
        printf("Network ID: %d.%d.0.0\n", p1, p2);
        printf("Host ID: %d.%d\n", p3, p4);
    }
    else if (p1 >= 192 && p1 <= 223) {
        printf("Class C\n");
        printf("Network ID: %d.%d.%d.0\n", p1, p2, p3);
        printf("Host ID: %d\n", p4);
    }
    else if (p1 >= 224 && p1 <= 239) {
        printf("Class D (Multicast)\n");
        printf("No host/network separation\n");
    }
    else if (p1 >= 240 && p1 <= 255) {
        printf("Class E (Reserved)\n");
        printf("No host/network separation\n");
    }
    else {
        printf("Invalid IP\n");
    }
}

int main()
{
    char input[25];
    printf("Enter IP: ");
    scanf("%s", input);
    find_class_and_network(input);
    return 0;
}