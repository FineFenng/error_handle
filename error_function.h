#ifdef __GNUC__
#define NORETURN __attribute__((__noreturn__))
#endif

void ErrorMesssage(const char* format, ...) NORETURN;

void ErrorExit(const char* format, ...) NORETURN;

void ErrorSysExit(const char* format, ...) NORETURN;

void ErrorExitWithErrno(int saved_errno, const char* format, ...) NORETURN;

void Fatal(const char* format, ...) NORETURN;

void UsageError(const char* format, ...) NORETURN;

void CommandLineError(const char* format, ...) NORETURN;











