// DIAGNOSTIC: plain D, not ImportC. Confirms whether catching a thrown
// exception inside an extern(C)-linkage function crashes the same way on
// Windows LDC, isolating whether this is about extern(C) linkage itself
// rather than anything ImportC-parsing-specific.

extern(C) int caughtPath()
{
    try
    {
        throw new Exception("test");
    }
    catch (Throwable __seh)
    {
        return 2;
    }
}

void main()
{
    assert(caughtPath() == 2);
}
