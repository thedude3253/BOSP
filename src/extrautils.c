int stringEqu(char *s1, char *s2) {
    while(*s1) {
        if(*s1 != *s2) return 0;
        s1++;
        s2++;
    }
    return 1;
}