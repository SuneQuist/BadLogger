#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

HHOOK keyboardHook;
HHOOK mouseHook;
FILE *logFile;
clock_t clock_before;
bool first_click = false;

// Function to write to log file
void writeToLog(const char *str) {
    if (logFile) {
        clock_t difference = clock() - clock_before;
        int time = difference * 1000 / CLOCKS_PER_SEC;
        clock_before = clock();

        if ( !first_click ) {
            fprintf(logFile, "%s\n", str);
            first_click = true;
        } else {
            fprintf(logFile, "S%d\n%s\n", time, str);
        }

        fflush(logFile);
    }
}

// Callback function for keyboard events
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT *)lParam;
            DWORD vkCode = kbdStruct->vkCode;

            char buffer[100];
            sprintf(buffer, "K%lu", vkCode);
            writeToLog(buffer);
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

// Callback function for mouse events
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN) {
            MOUSEHOOKSTRUCT* mouse = (MOUSEHOOKSTRUCT*) lParam;
            POINT coord = mouse->pt;

            char buffer[100];
            snprintf(buffer, sizeof(buffer), "M%ld,X%ld,Y%ld", wParam, coord.x, coord.y);
            writeToLog(buffer);
        }
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

void record() {
    clock_before = clock();

    // Open the log file
    logFile = fopen("keymouse_log.txt", "w");
    if (!logFile) {
        printf("Error opening log file.\n");
        return;
    }

    // Set the hooks
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);

    if (!keyboardHook || !mouseHook) {
        printf("Error setting hooks.\n");
        return;
    }

    // Message loop to keep the hooks running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook and clean up
    UnhookWindowsHookEx(keyboardHook);
    UnhookWindowsHookEx(mouseHook);
    fclose(logFile);
}

int main(int argc, char* argv[]) {
    if (strlen(argv[1]) == 0) { return 0; }

    if (strcmp(argv[1], "record") == 0) { record(); }
    if (strcmp(argv[1], "play") == 0) { player(); }

    return 0;
}
