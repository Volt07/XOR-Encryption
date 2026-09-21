#define _CTR_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void xorEncrypt(char *message, const char *key) {
    int keyLen = strlen(key);
    for (int i = 0; message[i] != '\0'; i++) {
        message[i] ^= key[i % keyLen];
    }
}

void encryptFile(const char *filename, char *message, const char *key) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file for writing.\n");
        return;
    }

    xorEncrypt(message, key);
    fprintf(file, "%s", message);

    fclose(file);
}

void decryptFile(const char *filename, const char *key) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file for reading.\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *encryptedData = (char *)malloc((fileSize + 1) * sizeof(char));
    if (!encryptedData) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return;
    }

    fread(encryptedData, sizeof(char), fileSize, file);
    encryptedData[fileSize] = '\0';

    xorEncrypt(encryptedData, key);

    for (int i = 0; encryptedData[i] != '\0'; i++) {
        printf("%c", encryptedData[i]);
    }
    printf("\n");

    free(encryptedData);
    fclose(file);
}

int main() {
    char choice = '\0';

    /* Prompt until user enters 'e' or 'd' */
    while (1) {
        printf("Enter 'e' to encrypt or 'd' to decrypt: ");
        if (scanf(" %c", &choice) != 1) {
            /* clear input */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Invalid input.\n");
            continue;
        }
        /* consume the rest of the line */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}

        if (choice == 'e' || choice == 'd') break;
        printf("Invalid choice. Please enter 'e' or 'd'.\n");
    }

    if (choice == 'e') {
        char message[1024];
        char filename[260];
        char key[256];

        /* Get non-empty message */
        while (1) {
            printf("Enter the message to encrypt: ");
            if (!fgets(message, sizeof(message), stdin)) {
                printf("Input error.\n");
                return 1;
            }
            message[strcspn(message, "\n")] = '\0';
            if (strlen(message) == 0) {
                printf("Message cannot be empty. Please enter text.\n");
                continue;
            }
            break;
        }

        /* Get filename; check if exists and confirm overwrite */
        while (1) {
            printf("Enter the filename to save encrypted data: ");
            if (scanf("%259s", filename) != 1) {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                printf("Invalid filename.\n");
                continue;
            }
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}

            FILE *f = fopen(filename, "r");
            if (f) {
                fclose(f);
                char resp = '\0';
                while (1) {
                    printf("File '%s' already exists. Overwrite? (y/n): ", filename);
                    if (scanf(" %c", &resp) != 1) {
                        while ((ch = getchar()) != '\n' && ch != EOF) {}
                        printf("Invalid input.\n");
                        continue;
                    }
                    while ((ch = getchar()) != '\n' && ch != EOF) {}
                    if (resp == 'y' || resp == 'Y') break;
                    if (resp == 'n' || resp == 'N') break;
                    printf("Please enter 'y' or 'n'.\n");
                }
                if (resp == 'y' || resp == 'Y') break;
                /* else ask for filename again */
                continue;
            }
            /* file does not exist, accept */
            break;
        }

        /* Get non-empty key */
        while (1) {
            printf("Enter the encryption key: ");
            if (!fgets(key, sizeof(key), stdin)) {
                printf("Input error.\n");
                return 1;
            }
            key[strcspn(key, "\n")] = '\0';
            if (strlen(key) == 0) {
                printf("Key cannot be empty. Please enter a key.\n");
                continue;
            }
            break;
        }

        encryptFile(filename, message, key);
    } else if (choice == 'd') {
        char filename[260];
        char key[256];

        /* Prompt for an existing filename */
        while (1) {
            printf("Enter the filename to decrypt: ");
            if (scanf("%259s", filename) != 1) {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                printf("Invalid filename.\n");
                continue;
            }
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}

            FILE *f = fopen(filename, "r");
            if (!f) {
                printf("File '%s' not found. Please enter a different filename.\n", filename);
                continue;
            }
            fclose(f);
            break;
        }

        /* Get non-empty key */
        while (1) {
            printf("Enter the decryption key: ");
            if (!fgets(key, sizeof(key), stdin)) {
                printf("Input error.\n");
                return 1;
            }
            key[strcspn(key, "\n")] = '\0';
            if (strlen(key) == 0) {
                printf("Key cannot be empty. Please enter a key.\n");
                continue;
            }
            break;
        }

        decryptFile(filename, key);
    }

    return 0;
}