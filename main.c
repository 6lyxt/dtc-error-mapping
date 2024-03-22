#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JSON_FILE "dtc.json"
#define MAX_ERROR_CODE_LENGTH 10
#define MAX_ERROR_MESSAGE_LENGTH 100

char* getErrorMessage(const char* error_code) {
    FILE *file = fopen(JSON_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to open JSON file.\n");
        return NULL;
    }

    char buffer[1024];
    char *error_message = NULL;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char *code_ptr = strstr(buffer, error_code);
        if (code_ptr != NULL) {
            char *message_ptr = strstr(buffer, "\"") + 1;
            char *end_ptr = strstr(message_ptr, "\"");
            size_t message_length = end_ptr - message_ptr;

            error_message = (char*)malloc((message_length + 1) * sizeof(char));
            strncpy(error_message, message_ptr, message_length);
            error_message[message_length] = '\0';

            break;
        }
    }

    fclose(file);
    return error_message;
}

int main() {
    char error_code[MAX_ERROR_CODE_LENGTH];

    printf("Enter the error code (e.g., P0379): ");
    scanf("%s", error_code);

    char *error_message = getErrorMessage(error_code);
    if (error_message != NULL) {
        printf("Error Message: %s\n", error_message);
        free(error_message);
    } else {
        printf("Error: Error code '%s' not found.\n", error_code);
    }

    return 0;
}