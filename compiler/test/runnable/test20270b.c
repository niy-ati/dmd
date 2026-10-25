/* https://issues.dlang.org/show_bug.cgi?id=20270
 * Taking the address of a __declspec(dllimport) function in a static
 * initializer must not be rejected at compile time - this is exactly the
 * shape of sqlite's os_win.c syscall table (a static array of function
 * pointers to real Windows API functions). On the actual Windows target
 * the backend defers writing the loader-resolved address into the data
 * slot until program startup (see glue.mscoffobj.write_importTableRef);
 * calling through the resulting pointer must still behave correctly.
 */

__declspec(dllimport) int __stdcall foo(void);

struct S { int (*fp)(void); };

struct S s = { &foo };

int __stdcall foo(void)
{
    return 42;
}

int main(void)
{
    return s.fp() == 42 ? 0 : 1;
}
