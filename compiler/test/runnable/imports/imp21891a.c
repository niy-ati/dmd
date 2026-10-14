// Minified from sljit's test_src/sljitMain.c

union executable_code21891 {
    void* code;
    long (*func)(long a);
};

union executable_code21891 make_a21891(void* p)
{
    union executable_code21891 u;
    u.code = p;
    return u;
}
