// EXTRA_SOURCES: imports/importc_seh.d
// DIAGNOSTIC: isolates __try/__except alone, with a real thrown exception,
// no nesting.

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
