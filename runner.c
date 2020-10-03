#include <stdio.h>

int main() {
    int a = 0, b = 0;
    for (int i = 0; i < 1234567; i++) {
        for (int j = 0; j < 66666; j++) {
            a = a + 1;
            b = b + a;
        }
    }
    printf("finished\n");
    return 0;
}