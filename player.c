#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

// Util
bool StartsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

// Util
void RemoveFirstChar(char* str) {
    // Check if the string is not empty
    if (str != NULL && str[0] != '\0') {
        // Shift all characters one position to the left
        for (int i = 0; str[i] != '\0'; i++) {
            str[i] = str[i + 1];
        }
    }
}

void MoveMouseTo(int x, int y) {
    SetCursorPos(x, y);
}

void MouseClick() {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN; // Left button down
    SendInput(1, &input, sizeof(INPUT));

    SecureZeroMemory(&input, sizeof(INPUT)); // Clear input
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

void PressKey(int k) {
    WORD key = (WORD)k;

    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    SendInput(1, &input, sizeof(INPUT));

    SecureZeroMemory(&input, sizeof(INPUT));
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void GetFileContents() {
    FILE* file = fopen("keymouse_log.txt", "r");
    int len = 256;
    char line[len];

    if (!file) {
        printf("Error opening log file.\n");
        return;
    }

    while(fgets(line, len, file) != NULL) {
            bool isMouse = false;
            bool isKey = false;
            bool isSleep = false;
            int sleepFor = 0;
            int keyToLog = -1;
            int* x;
            int* y;

            char* token = strtok(line, ",");
            while(token != NULL) {
                if ( StartsWith(token, "S") ) {
                    isSleep = true;
                    RemoveFirstChar(token);
                    sleepFor = atoi(token);
                }

                if ( StartsWith(token, "M") ) {
                    isMouse = true;
                }

                if ( isMouse && StartsWith(token, "X") ) {
                    RemoveFirstChar(token);
                    x = atoi(token);
                }

                if ( isMouse && StartsWith(token, "Y") ) {
                    RemoveFirstChar(token);
                    y = atoi(token);
                }

                if ( StartsWith(token, "K") ) {
                    isKey = true;
                    RemoveFirstChar(token);
                    keyToLog = atoi(token);
                }

                token = strtok(NULL, ",");
            }

            if (isMouse) {
                MoveMouseTo(x, y);
                MouseClick();
            }

            if (isKey) {
                PressKey(keyToLog);
            }

            if (isSleep) {
                Sleep(sleepFor);
            }
    }

    fclose(file);
}

int player() {
    Sleep(3000);

    GetFileContents();

    return EXIT_SUCCESS;
}
