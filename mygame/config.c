#include <stdio.h>
#include <string.h>
#include "config.h"

char* val_colorScheme[] = {
	"white",
	"black"
};

char* val_startLevel[] = {
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9"
};

char* val_grid[] = {
	"on",
	"off"
};

SDL_Color SchemeSign[] = {
	{255, 255, 255, 255},
	{0, 0, 0, 255}
};

OPTION config_opt[] = {
	{"colorScheme", val_colorScheme, 2},
	{"startLevel", val_startLevel, 9},
	{"grid", val_grid, 2}
};

ITEM default_config[] = {
	{"colorScheme", COLORSCHEME, "white", 0},
	{"startLevel", STARTLEVEL, "1", 0},
	{"grid", GRID, "on", 0}
};

int indexof(char* str, char ch) { 
	char* p = strchr(str, ch);
	return (int)(p - str); 
}

void strSlice(char* dest, char* src, int start, int length) { 
	for (int i = 0; i < length; i++)
		dest[i] = src[start + i];
	dest[length] = 0; 
}

void parse_config(ITEM* config) {
	FILE* fp;
	fopen_s(&fp, CONFIG_FILE, "r");
	if (!fp) {
		printf("Filed to open file\n");
		exit(-1);
	}
	char buf[100], opt[50], val[50];
	int sp, value = -1, size;
	size_t length;
	while (fgets(buf, 100, fp) != NULL) {
		sp = indexof(buf, DELIM);
		length = strlen(buf);
		strSlice(opt, buf, 0, sp);
		strSlice(val, buf, sp + 1, length - 2 - sp);
		enum CONFIG optName = COLORSCHEME;
		while (strcmp(config[optName].optStr, opt) != 0)
			optName++;
		size = config_opt[optName].size;
		for (int i = 0; i < size; i++) {
			if (strcmp(config_opt[optName].value[i], val) == 0) {
				value = i;
				break;
			}
		}
		if (value != -1) {
			config[optName].valueStr = config_opt[optName].value[value];
			config[optName].value = value;
		}
	}
	fclose(fp);
}

void save_config(ITEM* config) {
	FILE* fp;
	fopen_s(&fp, CONFIG_FILE, "w");
	if (!fp) {
		printf("Failed to open config file.\n");
		exit(-1);
	}
	for (int i = 0; i < OPTSIZE; i++) {
		fprintf(fp, "%s %s\n", config[i].optStr, config[i].valueStr);
	}
}