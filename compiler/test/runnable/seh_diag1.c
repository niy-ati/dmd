// DIAGNOSTIC: isolates __try/__finally alone, no exception at all.

int finallyRan;

int normalPath(void)
{
    __try {
        return 1;
    } __finally {
        finallyRan = 1;
    }
}

int main(void)
{
    if (normalPath() != 1) return 1;
    if (!finallyRan) return 2;
    return 0;
}
