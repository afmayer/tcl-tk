/*
 * tkStubLib.c --
 *
 *	Stub object that will be statically linked into extensions that wish
 *	to access Tk.
 *
 * Copyright (c) 1998 Paul Duffin.
 * Copyright (c) 1998-1999 by Scriptics Corporation.
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

/*
 * We need to ensure that we use the stub macros so that this file contains no
 * references to any of the stub functions. This will make it possible to
 * build an extension that references Tk_InitStubs but doesn't end up
 * including the rest of the stub functions.
 */

#undef USE_TCL_STUBS
#define USE_TCL_STUBS

#define USE_TK_STUBS

#include "tkInt.h"

#ifdef __WIN32__
#include "tkWinInt.h"
#endif

#ifdef MAC_OSX_TK
#include "tkMacOSXInt.h"
#endif

#if !(defined(__WIN32__) || defined(MAC_OSX_TK))
#include "tkUnixInt.h"
#endif

/* TODO: These ought to come in some other way */
#include "tkPlatDecls.h"
#include "tkIntXlibDecls.h"

MODULE_SCOPE const TkStubs *tkStubsPtr;
MODULE_SCOPE const TkPlatStubs *tkPlatStubsPtr;
MODULE_SCOPE const TkIntStubs *tkIntStubsPtr;
MODULE_SCOPE const TkIntPlatStubs *tkIntPlatStubsPtr;
MODULE_SCOPE const TkIntXlibStubs *tkIntXlibStubsPtr;

const TkStubs *tkStubsPtr = NULL;
const TkPlatStubs *tkPlatStubsPtr = NULL;
const TkIntStubs *tkIntStubsPtr = NULL;
const TkIntPlatStubs *tkIntPlatStubsPtr = NULL;
const TkIntXlibStubs *tkIntXlibStubsPtr = NULL;

/*
 * Use our own isdigit to avoid linking to libc on windows
 */

static int
isDigit(
    const int c)
{
    return (c >= '0' && c <= '9');
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_InitStubs --
 *
 *	Checks that the correct version of Tk is loaded and that it supports
 *	stubs. It then initialises the stub table pointers.
 *
 * Results:
 *	The actual version of Tk that satisfies the request, or NULL to
 *	indicate that an error occurred.
 *
 * Side effects:
 *	Sets the stub table pointers.
 *
 *----------------------------------------------------------------------
 */

MODULE_SCOPE const char *
Tk_InitStubs(
    Tcl_Interp *interp,
    const char *version,
    int exact)
{
    ClientData pkgClientData = NULL;
    const char *actualVersion = Tcl_PkgRequireEx(interp, "Tk", version, 0,
	    &pkgClientData);
    const TkStubs *stubsPtr = pkgClientData;

    if (!actualVersion) {
	return NULL;
    }
    if (exact) {
        const char *p = version;
        int count = 0;

        while (*p) {
            count += !isDigit(*p++);
        }
        if (count == 1) {
	    const char *q = actualVersion;

	    p = version;
	    while (*p && (*p == *q)) {
		p++; q++;
	    }
            if (*p) {
		/* Construct error message */
		Tcl_PkgRequireEx(interp, "Tk", version, 1, NULL);
                return NULL;
            }
        } else {
            actualVersion = Tcl_PkgRequireEx(interp, "Tk", version, 1, NULL);
            if (actualVersion == NULL) {
                return NULL;
            }
        }
    }

    if (!stubsPtr) {
	Tcl_SetObjResult(interp, Tcl_NewStringObj(
		"this implementation of Tk does not support stubs", -1));
	return NULL;
    }

    tkPlatStubsPtr = stubsPtr->hooks->tkPlatStubs;
    tkIntStubsPtr = stubsPtr->hooks->tkIntStubs;
    tkIntPlatStubsPtr = stubsPtr->hooks->tkIntPlatStubs;
    tkIntXlibStubsPtr = stubsPtr->hooks->tkIntXlibStubs;
    tkStubsPtr = stubsPtr;

    return actualVersion;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */
