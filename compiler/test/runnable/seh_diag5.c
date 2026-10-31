// EXTRA_SOURCES: imports/importc_seh.d
// DIAGNOSTIC: simplest possible ImportC __try/__except with a real thrown
// exception, verifying the catch-variable-linkage fix against a real .c
// file (isCsymbol() only returns true for symbols from an actual C file).

__import importc_seh;

int caughtPath(void)
{
    __try {
        throwIt();
        return 0; // unreached
    } __except(1) {
        return 2;
    }
}

int main(void)
{
    if (caughtPath() != 2) return 1;
    return 0;
}
