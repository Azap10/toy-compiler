#include <stdio.h>

// #define max_1(a,b) (a > b ? a : b)

// #define max_2(a,b) \
// ({ __typeof__ (a) _a = (a); \
// __typeof__ (b) _b = (b); \
// _a > _b ? _a : _b; })

int main (void) {
	int a = 5;
	int b = 4;
    if (b += 2 > a) {
        printf("%d\n", b);
    }
}
