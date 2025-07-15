// myprog.c
#include <stdio.h>

__attribute__((noinline))
void foo() {
    printf("Inside foo()\n");
}

__attribute__((noinline))
long fibonacci(unsigned n) {
    if (n < 2) return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

__attribute__((noinline))
long adder(unsigned n) {
    if (n < 2) return n;
    
    long result = 0;
    long prev = 2;
    for (int i = 2; i < n; ++i) {
        result += prev;
        prev = result;
    }
    
    return result;
}

int main() {
    foo();
    adder(6);
    foo();
    adder(13);
    foo();
    adder(24);
    return 0;
}
