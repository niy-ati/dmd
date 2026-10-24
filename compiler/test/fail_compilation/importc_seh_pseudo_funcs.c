/* TEST_OUTPUT:
---
fail_compilation/importc_seh_pseudo_funcs.c(19): Error: undefined identifier `_exception_code`
fail_compilation/importc_seh_pseudo_funcs.c(19): Error: undefined identifier `_exception_info`
---
 */

// _exception_code/_exception_info aren't implemented: the hardware-exception
// information they need isn't preserved by the time a Windows SEH fault
// becomes a catchable D exception, so using them gives a clear, honest error
// rather than a silently wrong value.

int sehExceptionFilter(void *p, int code, void *info);

int f(void *p)
{
    __try {
        return 0;
    } __except( sehExceptionFilter(p, _exception_code(), _exception_info()) ) {
        return 1;
    }
}
