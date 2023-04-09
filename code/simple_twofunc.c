long add2(long a, long b) {
    long c = a + b;
    return c;
}

long foo(long a, long b) {
    long c = 5 + b;
    long d = a + c;
    long e = add2(c, d);
    return e;
}

