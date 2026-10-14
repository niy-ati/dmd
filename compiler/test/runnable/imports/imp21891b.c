// Minified from sljit's test_src/sljitTest.c

union executable_code21891 {
    void* code;
    long (*func0)(void);
    long (*func1)(long a);
    long (*func2)(long a, long b);
    long (*func3)(long a, long b, long c);
};

union executable_code21891 make_b21891(void* p)
{
    union executable_code21891 u;
    u.code = p;
    return u;
}
