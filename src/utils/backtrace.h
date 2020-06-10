/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Backtrace_H_
#define _Backtrace_H_

#ifdef ANDROID

#define BACKTRACE() 

#else


#include "pdebug.h"
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>

#define BACKTRACE() PDEBUG << qPrintable(stacktrace(16))

static inline QString stacktrace(unsigned int max_frames = 63) {
    QString text = "Backtrace:\n";

    // storage array for stack trace address data
    void *addrlist[max_frames+1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));
    if (addrlen == 0) {
        text += " <empty, possibly corrupt>\n";
        return text;
        }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 1; i < addrlen; i++) {
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

        // find parentheses and +address offset surrounding the mangled name:
        // ./module(function+0x15c) [0x8048a6d]
        for (char *p = symbollist[i]; *p; ++p) {
            if (*p == '(') begin_name = p;
            else if (*p == '+') begin_offset = p;
            else if (*p == ')' && begin_offset) {
                end_offset = p;
                break;
                }
            }

        if (begin_name && begin_offset && end_offset && begin_name < begin_offset) {
            *begin_name++   = '\0';
            *begin_offset++ = '\0';
            *end_offset     = '\0';

            // mangled name is now in [begin_name, begin_offset) and caller
            // offset in [begin_offset, end_offset). now apply
            // __cxa_demangle():

            int status;
            char* ret = abi::__cxa_demangle(begin_name,
            funcname, &funcnamesize, &status);
            if (status == 0) {
                funcname = ret; // use possibly realloc()-ed string
                text += QString("  %1 : %2+%3\n")
                            .arg(symbollist[i])
                            .arg(funcname)
                            .arg(begin_offset)
                            ;
              } else {
                // demangling failed. Output function name as a C function with
                // no arguments.
                text += QString("  %1 : %2+%3\n")
                            .arg(symbollist[i])
                            .arg(begin_name)
                            .arg(begin_offset)
                            ;
                }
          } else {
            // couldn't parse the line? print the whole line.
            text += QString("  %1\n").arg(symbollist[i]);
            }
    }

    free(funcname);
    free(symbollist);
    return text;
}


#endif
#endif
