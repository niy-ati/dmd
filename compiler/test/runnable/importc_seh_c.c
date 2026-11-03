// EXTRA_SOURCES: imports/importc_seh.d
// DISABLED: win64
// Verifies __try/__except/__finally, lowered to try/catch/finally.
//
// Disabled on win64: when dmd is bootstrapped with LDC as host compiler
// (CI's "Windows x64, LDC" job), the resulting dmd.exe crashes running
// this - reproduced locally with the real LDC 1.43.0 release matching CI.
// The crash dump shows execution landing on a 2-byte `hlt` stub that is
// registered as its own function in the PE unwind table, not on real
// code. Bootstrapping with any other host compiler produces a dmd.exe
// that runs this correctly, so this is LDC miscompiling something in
// dmd's own backend during the bootstrap build, not a frontend bug.

__import importc_seh;

int finallyRanOnNormalReturn;
int finallyRanOnException;

int normalPath(void)
{
    __try {
        return 1;
    } __finally {
        finallyRanOnNormalReturn = 1;
    }
}

int caughtPath(void)
{
    __try {
        throwIt();
        return 0; // unreached
    } __except(1) {
        return 2;
    }
}

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
    if (normalPath() != 1) return 1;
    if (!finallyRanOnNormalReturn) return 2;

    if (caughtPath() != 2) return 3;

    if (finallyRunsOnException() != 3) return 4;
    if (!finallyRanOnException) return 5;

    return 0;
}
