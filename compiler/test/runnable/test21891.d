// https://github.com/dlang/dmd/issues/21891

import imports.imp21891a;
import imports.imp21891b;

void main()
{
    auto a = make_a21891(null);
    auto b = make_b21891(null);
    assert(a.code is null);
    assert(b.code is null);
}
