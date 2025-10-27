#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void xorOperation(char *dividend, const char *divisor, int pos, int len) {
    for (int i = 0; i < len; i++) {
        dividend[pos + i] = (dividend[pos + i] == divisor[i]) ? '0' : '1';
    }
}

void computeCRC(char data[], char generator[], char remainder[]) {
    int dataLen = strlen(data);
    int genLen = strlen(generator);
    char temp[100];
    strcpy(temp, data);


    for (int i = 0; i < genLen - 1; i++)
        strcat(temp, "0");

    int totalLen = dataLen + genLen - 1;
    for (int i = 0; i <= totalLen - genLen; i++) {
        if (temp[i] == '1') {
            xorOperation(temp, generator, i, genLen);
        }
    }

    strncpy(remainder, temp + totalLen - genLen + 1, genLen - 1);
    remainder[genLen - 1] = '\0';
}

void introduceError(char data[], int positions[], int count) {
    for (int i = 0; i < count; i++) {
        int pos = positions[i];
        if (pos >= 0 && pos < strlen(data)) {
            data[pos] = (data[pos] == '0') ? '1' : '0';
        }
    }
}

int checkError(char received[], char generator[]) {
    int genLen = strlen(generator);
    char temp[100];
    strcpy(temp, received);

    int totalLen = strlen(temp);
    for (int i = 0; i <= totalLen - genLen; i++) {
        if (temp[i] == '1') {
            xorOperation(temp, generator, i, genLen);
        }
    }


    for (int i = totalLen - genLen + 1; i < totalLen; i++) {
        if (temp[i] == '1') return 1;
    }
    return 0;
}

int main() {
    char data[100], generator[100], remainder[100], transmitted[200];
    int errorPositions[10], errorCount;
    FILE *fp = fopen("output.txt", "w");


    printf("Enter the data bits: ");
    scanf("%s", data);


    printf("Enter the generator polynomial: ");
    scanf("%s", generator);


    computeCRC(data, generator, remainder);


    strcpy(transmitted, data);
    strcat(transmitted, remainder);

    fprintf(fp, "Original Data: %s\n", data);
    fprintf(fp, "Generator Polynomial: %s\n", generator);
    fprintf(fp, "CRC Checksum: %s\n", remainder);
    fprintf(fp, "Transmitted Data: %s\n", transmitted);

    printf("Enter number of error positions: ");
    scanf("%d", &errorCount);

    printf("Enter error positions (0-based index): ");
    for (int i = 0; i < errorCount; i++) {
        scanf("%d", &errorPositions[i]);
    }


    introduceError(transmitted, errorPositions, errorCount);

    fprintf(fp, "\nErrored Data: %s\n", transmitted);

    if (checkError(transmitted, generator)) {
        fprintf(fp, "Error detected using CRC method.\n");
    } else {
        fprintf(fp, "No error detected.\n");
    }

    fclose(fp);
    printf("Output written to output.txt\n");
    return 0;
}
