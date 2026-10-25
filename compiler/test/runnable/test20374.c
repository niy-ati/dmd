/* https://issues.dlang.org/show_bug.cgi?id=20374
 * __va_start is the MSVC intrinsic that real vadefs.h/stdarg.h define
 * va_start(ap, lastparam) in terms of, on x64. It must be rewritten at
 * parse time into a call to core.stdc.stdarg.va_start that runs directly
 * in the calling function - a library wrapper function would bind the
 * va_list to the wrapper's own stack frame instead of the real caller's,
 * silently reading garbage.
 */

#define va_list __builtin_va_list
#define va_start(ap, x) __va_start(&ap, x)
#define va_arg(ap, t) __builtin_va_arg(ap, t)

int sum(int count, ...)
{
    va_list ap;
    va_start(ap, count);
    int total = 0;
    for (int i = 0; i < count; i++)
        total += va_arg(ap, int);
    return total;
}

double mix(int n, ...)
{
    va_list ap;
    va_start(ap, n);
    double total = 0;
    for (int i = 0; i < n; i++)
    {
        if (i % 2 == 0)
            total += va_arg(ap, int);
        else
            total += va_arg(ap, double);
    }
    return total;
}

int main(void)
{
    if (sum(3, 10, 20, 12) != 42)
        return 1;
    if (mix(4, 1, 2.5, 3, 4.5) != 11.0)
        return 2;
    return 0;
}
