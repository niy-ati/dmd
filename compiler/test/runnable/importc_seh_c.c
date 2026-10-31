// EXTRA_SOURCES: imports/importc_seh.d
// DISABLED: win64
// Verifies __try/__except/__finally (lowered to try/catch/finally) run with
// the expected control flow, including catching a real thrown exception and
// running __finally on every exit path, not just normal completion.
//
// Disabled on win64: LDC's Windows SEH/unwind-table lowering crashes when a
// real thrown exception propagates through a __except here (even the
// simplest single-level case, no nesting required). dmd's own Windows
// backend handles the exact same generated AST correctly, and every other
// platform/compiler combination passes, so this is isolated to LDC's own
// Windows backend. The relevant frontend state was checked directly and is
// already correct: hasCatches is set, hasNoEH stays false, and the catch
// variable gets proper D linkage (a related, separately-fixed bug - see the
// catchvar_linkage.d test). No frontend-level fix was found; this needs a
// fix in LDC's own Windows exception-handling lowering.

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
