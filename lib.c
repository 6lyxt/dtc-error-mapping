#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSON_FILE "dtc.json"
#define MAX_ERROR_CODE_LENGTH 10
#define MAX_ERROR_MESSAGE_LENGTH 100
#define MAX_LINE_LENGTH 256

int main() {
    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char *errorStart = strchr(line, '[');
        if (errorStart != NULL) {
            errorStart++;
            char *errorEnd = strchr(errorStart, ']');
            if (errorEnd != NULL) {
                *errorEnd = '\0';
                char *errorCodes = strtok(errorStart, ", ");
                
                while (errorCodes != NULL) {
                    char *errorMessage = getErrorMessage(errorCodes);
                    if (errorMessage != NULL) {
                        printf("Error Code %s: %s\n", errorCodes, errorMessage);
                    } else {
                        printf("Error Code %s: Unknown error\n", errorCodes);
                    }
                    errorCodes = strtok(NULL, ", ");
                }
            }
        }
    }

    fclose(file);
    return 0;
}


char* getErrorMessage(const char* error_code) {
    FILE *file = fopen(JSON_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to open JSON file.\n");
        return NULL;
    }

    char buffer[1024];
    char *error_message = NULL;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *code_ptr = strstr(buffer, "\"Code\":");
        if (code_ptr != NULL) {
            code_ptr += strlen("\"Code\":");
            if (strncmp(code_ptr, error_code, strlen(error_code)) == 0) {
                char *message_ptr = strstr(buffer, "\"Message\":");
                if (message_ptr != NULL) {
                    message_ptr += strlen("\"Message\":");
                    message_ptr = strchr(message_ptr, '\"');
                    if (message_ptr != NULL) {
                        message_ptr++;
                        char *end_ptr = strchr(message_ptr, '\"');
                        if (end_ptr != NULL) {
                            size_t message_length = end_ptr - message_ptr;
                            error_message = (char*)malloc((message_length + 1) * sizeof(char));
                            strncpy(error_message, message_ptr, message_length);
                            error_message[message_length] = '\0';
                            break;
                        }
                    }
                }
            }
        }
    }

    fclose(file);
    return error_message;
}
