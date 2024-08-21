void memset(char* ptr, char value, int size) {
    int i;
    for (i = 0; i < size; i++) ptr[i] = value;
}

void memcpy(char* dest, const char* src, int size) {
    int i;
    for (i = 0; i < size; i++) dest[i] = src[i];
}