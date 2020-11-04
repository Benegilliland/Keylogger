#include <string>
#include <stdlib.h>
#include <time.h>
#include <fstream>

char get_key();
int encrypt_char(char& val, char ckey);
int encrypt_string(std::string& val, char ckey);

char get_key() {
	srand(time(0)); // Generate random seed
	char ckey = (char)(rand() % 256); // Generates random character
	return ckey;
}

int encrypt_char(char& val, char ckey) {
	val = ~val; // Reverse bits
	val ^= ckey; // Bitwise XOR
	val << 4; // Left shift 4 bits
	val += 5;
	return 0;
}

int encrypt_string(std::string& val, char ckey) {
	for (int i = 0; i < val.length(); i++) {
		char c = val[i];
		encrypt_char(c, ckey);
		val[i] = c;
	}
	return 0;
}
