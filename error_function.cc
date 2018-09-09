#include "error_function.h"

#include <unistd.h>

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ename.c.inc"

#ifdef __GNUC__
__attribute__ ((__noreturn__))
#endif

static void Terminate(bool use_exit_3)
{ const char* value = ::getenv("EF_DUMCORE");

    /* Dump core if EF_DUMPCORE environment variable is defined and
     * is a nonempty string; otherwise call exit(3) or _exit(2),
     * depending on the value of 'uesExit3'.
     */

    if (value != nullptr && *value != '\0') {
        abort();
    } else if (use_exit_3) {
        exit(EXIT_FAILURE);
    } else {
        _exit(EXIT_FAILURE);
    }
}

static void OutputError(bool is_use_errno, int saved_errno, bool is_flush_stdout,
    const char* format, va_list ag)
{
#define BUFF_SIZE 500

    char buff[BUFF_SIZE];
    char user_msg[BUFF_SIZE];
    char error_text[BUFF_SIZE];

    vsnprintf(user_msg, BUFF_SIZE, format, ag);

    char error_buffer[64] = { 0 };

    strerror_r(saved_errno, error_buffer, sizeof(error_buffer));

    if (is_use_errno) {
        snprintf(error_text, BUFF_SIZE, "[%s %s]",
            (saved_errno > 0 && saved_errno <= 255) ? ename[saved_errno] : "?UNKOWN?",
            error_buffer);
    } else {
        snprintf(error_text, BUFF_SIZE, ":");
    }
    snprintf(buff, BUFF_SIZE, "ERROR%s %s\n", error_text, user_msg);

    if (is_flush_stdout) {
        fflush(stdout);
    }
    fputs(buff, stderr);
    fflush(stderr);
}

void ErrorMessage(const char* format, ...)
{
    va_list args;
    int saved_errno = errno;

    va_start(args, format);
    OutputError(true, saved_errno, true, format, args);
    va_end(args);
    errno = saved_errno;
}

void ErrorExit(const char* format, ...)
{
    va_list args;

    int saved_errno = errno;

    va_start(args, format);
    OutputError(true, saved_errno, true, format, args);
    va_end(args);

    Terminate(true);
}

void ErrorSysExit(const char* format, ...)
{
    va_list args;

    int saved_errno = errno;
    va_start(args, format);
    OutputError(true, saved_errno, false, format, args);
    va_end(args);

    Terminate(false);
}

void errorExitWithErrno(int saved_errno, const char* format, ...)
{
    va_list args;

    va_start(args, format);
    OutputError(true, saved_errno, false, format, args);
    va_end(args);

    Terminate(true);
}

void Fatal(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    OutputError(false, 0, true, format, args);
    va_end(args);
    Terminate(true);
}

void UsageError(const char* format, ...)
{
    va_list args;

    fflush(stdout);

    fprintf(stderr, "Usage: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fflush(stderr);
    exit(EXIT_FAILURE);
}

void CommandLineError(const char* format, ...)
{
    va_list args;

    fflush(stdout);

    fprintf(stderr, "Command-line usage error: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fflush(stderr);
    exit(EXIT_FAILURE);
}
