// EXTRA_SOURCES: imports/importc_seh.d
// DIAGNOSTIC: isolates a __try/__finally nested inside a __try/__except,
// with the exception thrown from the innermost try.

__import importc_seh;

int finallyRanOnException;

int finallyRunsOnException(void)
{
    __try {
        __try {
            throwIt();
        } __finally {
            finallyRanOnException = 1;
        }
        return 0; // unreached
    } __except(1) {
        return 3;
    }
}

int main(void)
{
    if (finallyRunsOnException() != 3) return 1;
    if (!finallyRanOnException) return 2;
    return 0;
}
