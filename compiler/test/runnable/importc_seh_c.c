// EXTRA_SOURCES: imports/importc_seh.d
// DISABLED: win64
// Verifies __try/__except/__finally, lowered to try/catch/finally.
//
// Disabled on win64: when dmd itself is bootstrapped with LDC (CI's
// "Windows x64, LDC" job), the resulting dmd.exe emits a wrong filter-check
// stub for __except's condition - confirmed via a local repro built the
// same way CI does (dmd.exe self-built with the real LDC 1.43.0 release),
// the crash dump shows the exception dispatcher jumping straight into a
// 2-byte `hlt; ret` stub that is registered as its own function in the
// PE unwind table (dumpbin /unwindinfo), in place of the real filter body.
// dmd's own backend does not use __CxxFrameHandler3 at all for this (every
// unwind entry in the binary has no EH handler flag set; that mechanism is
// only used when LDC itself compiles D code, not by dmd's native backend),
// so this is LDC miscompiling a routine in dmd's own backend source during
// the bootstrap build, not an ImportC or frontend bug. No fix is possible
// in this repo; needs a fix in LDC itself.

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
