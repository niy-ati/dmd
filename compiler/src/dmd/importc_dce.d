/**
 * Dead-code elimination for unreferenced ImportC `inline`/`__forceinline` functions.
 *
 * C expects an unused `inline`/`__forceinline` function definition to produce
 * no standalone code at all. ImportC instead emits every function it parses
 * unconditionally, which breaks linking when such a function calls something
 * that is itself never defined (as happens with some `<windows.h>` inline
 * wrappers). This scans the whole program, once semantic analysis is done,
 * for `inline`/`__forceinline` C functions that turn out to be genuinely
 * unreferenced, and marks them to skip codegen.
 *
 * Copyright:   Copyright (C) 1999-2026 by The D Language Foundation, All Rights Reserved
 * License:     $(LINK2 https://www.boost.org/LICENSE_1_0.txt, Boost License 1.0)
 * Source:    $(LINK2 https://github.com/dlang/dmd/blob/master/compiler/src/dmd/importc_dce.d, _importc_dce.d)
 */

module dmd.importc_dce;

import dmd.aggregate;
import dmd.arraytypes : Modules;
import dmd.astenums : PINLINE;
import dmd.attrib;
import dmd.dclass;
import dmd.declaration;
import dmd.dmodule;
import dmd.dsymbol;
import dmd.dtemplate;
import dmd.expression;
import dmd.func;
import dmd.init;
import dmd.statement;
import dmd.visitor;

/// Functions seen while parsing that might qualify; checked for real once
/// semantic analysis of the whole program is done, in scanImportCInlineUsage.
private __gshared FuncDeclaration[] importCInlineCandidates;

/***********************************************************
 * Register fd as a possible ImportC inline/__forceinline dead-code
 * candidate. Called from the parser, before fd.fbody is attached; fbody
 * and the other qualifying conditions are checked later, for real, in
 * scanImportCInlineUsage().
 */
void registerImportCInlineCandidate(FuncDeclaration fd)
{
    importCInlineCandidates ~= fd;
}

/***********************************************************
 * After semantic analysis of the whole program is done, mark any
 * ImportC inline/__forceinline function that is never referenced
 * anywhere in the program so it is not codegen'd.
 *
 * Params:
 *   modules = every module in the program
 */
void scanImportCInlineUsage(Modules* modules)
{
    FuncDeclaration[] candidates;
    foreach (fd; importCInlineCandidates)
    {
        if (fd.fbody && fd.inlining == PINLINE.always && fd.isCsymbol())
            candidates ~= fd;
    }
    importCInlineCandidates = null;

    if (!candidates.length)
        return;

    scope v = new ImportCInlineUsageVisitor(candidates);
    foreach (m; *modules)
        visitDsymbolTree(m, v);

    foreach (fd; candidates)
        if (fd !in v.referenced)
            fd.skipCodegen = true;
}

/// Walk a Dsymbol tree, visiting every function body and variable
/// initializer with v, recursing into anything that can hold either.
private void visitDsymbolTree(Dsymbol s, ImportCInlineUsageVisitor v)
{
    if (auto fd = s.isFuncDeclaration())
    {
        if (fd.fbody)
            fd.fbody.accept(v);
        return;
    }
    if (auto vd = s.isVarDeclaration())
    {
        if (auto ei = vd._init ? vd._init.isExpInitializer() : null)
            ei.exp.accept(v);
        return;
    }
    if (auto ad = s.isAttribDeclaration())
    {
        if (ad.decl)
            foreach (s2; *ad.decl)
                visitDsymbolTree(s2, v);
        return;
    }
    if (auto sds = s.isScopeDsymbol())
    {
        if (sds.members)
            foreach (s2; *sds.members)
                visitDsymbolTree(s2, v);
        return;
    }
}

private extern (C++) final class ImportCInlineUsageVisitor : SemanticTimeTransitiveVisitor
{
    alias visit = SemanticTimeTransitiveVisitor.visit;

    bool[FuncDeclaration] isCandidate;
    bool[FuncDeclaration] referenced;

    extern (D) this(FuncDeclaration[] candidates) scope
    {
        foreach (fd; candidates)
            isCandidate[fd] = true;
    }

    // VarExp/SymOffExp have no dedicated override point in this visitor
    // hierarchy, so this catches them (and everything else) via their
    // common Expression base instead.
    override void visit(Expression e)
    {
        if (auto ve = e.isVarExp())
            mark(ve.var);
        else if (auto se = e.isSymOffExp())
            mark(se.var);
    }

    // A direct call's resolved callee is CallExp.f, set independently of
    // e1 once semantic analysis devirtualizes/resolves the call - e1 alone
    // (what the base Expression traversal above sees) cannot be relied on
    // to find it, so this is checked explicitly.
    override void visit(CallExp ce)
    {
        if (ce.f)
            mark(ce.f);
        super.visit(ce);
    }

    private void mark(Declaration d)
    {
        if (auto fd = d.isFuncDeclaration())
            if (fd in isCandidate)
                referenced[fd] = true;
    }
}
