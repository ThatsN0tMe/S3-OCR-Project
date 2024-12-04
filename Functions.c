int Min(int a, int b) {
    if (a > b) return b;
    return a;
}
int Max(int a, int b) {
    if (a > b) return a;
    return b;
}

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp; 
}