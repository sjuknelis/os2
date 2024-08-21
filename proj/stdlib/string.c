int strcmp(const char* str_a, const char* str_b) {
    int i = 0;
    while (str_a[i] != 0 && str_b[i] != 0) {
        if (str_a[i] != str_b[i]) return str_a[i] - str_b[i];
        i++;
    }

    if (str_a[i] != 0 || str_b[i] != 0) return str_a[i] - str_b[i];
    return 0;
}

void strcpy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != 0) dest[i] = src[i++];
    dest[i] = 0;
}

int strlen(const char* str) {
    int i = 0;
    while (str[i++] != 0);
    return i - 1;
}

int atoi(const char* str) {
    int value = 0, i = 0;
    char c;

    while (str[i] != 0) {
        c = str[i++];
        if (c >= '0' && c <= '9') value = value * 10 + (c - '0');
    }

    return value;
}

char* itoa(int value, char* str) {
    int i = 0, result_len = 0, copy_value = value;

    if (value == 0) {
        str[0] = '0';
        str[1] = 0;
        return str;
    }

    while (copy_value > 0) {
        result_len++;
        copy_value /= 10;
    }

    while (value > 0) {
        str[result_len - i++ - 1] = (value % 10) + '0';
        value /= 10;
    }
    str[result_len] = 0;

    return str;
}