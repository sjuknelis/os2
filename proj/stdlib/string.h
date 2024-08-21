#ifndef STRING_H
#define STRING_H

int strcmp(const char* strA, const char* strB);
void strcpy(char* dest, const char* src);
int strlen(const char* str);

int atoi(const char* str);
char* itoa(int value, char* str);

#endif