#include "font.h"

bool readFile(u8* buffer, const char* path) {
	FILE* file = fopen(path, "rb");
	if(file == NULL) return false;
	
	fseek(file,0,SEEK_END);
	off_t size = ftell(file);
	
	fseek(file,0,SEEK_SET);
	buffer = malloc(size);
	
	if(!buffer) return false;
	off_t bytesRead = fread(buffer,1,size,file);
	fclose(file);
	
	if(size!=bytesRead) return false;
	return true;
}

bool initFont() {
	bool IS_CIA = true; //IF YOU DO NOT WANT TO USE ROMFS, SWITCH THIS TO FALSE!!!
	
	g_letters[0] = "ABCDEFGHIJKLM";
	g_letters[1] = "NOPQRSTUVWXYZ";
	g_letters[2] = "abcdefghijklm";
	g_letters[3] = "nopqrstuvwxyz";
	g_letters[4] = "1234567890@- ";
	g_letters[5] = ";:'\"!,.+-[]/";
	
	if(IS_CIA) {
		Result rc = romfsInit();
		if (rc) {
			initConsole();
			printf("romfsInit: %08lX\n", rc);
			waitConsole();
			return false;
		}
		if(!readFile(g_font12, "romfs:/font12.bin") &&
			readFile(g_font24, "romfs:/font24.bin")) {
			return false;
		} 
		romfsExit();
	} else {
		if(!readFile(g_font12, "sdmc:/font12.bin") &&
			readFile(g_font24, "sdmc:/font24.bin")) {
			return false;
		}
	}
	return true;
}

void write12(Point p, const char* s) {
	
}
