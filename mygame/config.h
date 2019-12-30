#pragma once
#ifndef CONFIG_H_
#define CONFIG_H_

#define DELIM (' ')
#define CONFIG_FILE ("setting.ini")
#define OPTSIZE 3

#include <SDL.h>

enum CONFIG {
	COLORSCHEME,
	STARTLEVEL,
	GRID,
};

typedef struct {
	char* name;
	char** value;
	int size;
} OPTION;

typedef struct {
	char* optStr;
	int opt;
	char* valueStr;
	int value;
} ITEM;

extern ITEM default_config[];
extern OPTION config_opt[];
extern SDL_Color SchemeSign[];

int indexof(char*, char);
void strSlice(char*, char*, int, int);
void parse_config(ITEM*);
void save_config(ITEM*);

#endif