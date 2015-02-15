/*!
 * \file include/utils/strings.h
 * \brief String handling utilities
 *
 * \author xythobuz
 * \author Mongoose
 */

#include <cstdarg>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#if defined(unix) || defined(__APPLE__)
#include <wordexp.h>
#endif

#include "utils/strings.h"

bool stringEndsWith(const char *str, const char *suffix) {
    assert(str != NULL);
    assert(suffix != NULL);

    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);

    if (lensuffix > lenstr)
        return false;

    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

char *bufferString(const char *string, va_list args) {
    int sz = 60;
    int n;
    char *text;

    assert(string != NULL);
    assert(string[0] != '\0');

    text = new char[sz];

    va_list copy;
    va_copy(copy, args);

    n = vsnprintf(text, sz, string, args);

    if (n < 0) {
        delete [] text;
        return NULL; // encoding error
    } else if (n >= sz) {
        sz = n + 1; // buffer too small
        delete [] text;
        text = new char[sz];
        n = vsnprintf(text, sz, string, copy);
    }

    va_end(copy);

    return text;
}

char *bufferString(const char *string, ...) {
    va_list args;
    va_start(args, string);
    char *text = bufferString(string, args);
    va_end(args);
    return text;
}

char *fullPath(const char *path, char end) {
    unsigned int lenPath, offset;
    wordexp_t word;
    char *dir;

    assert(path != NULL);
    assert(path[0] != '\0');

    if (path[0] == '~') {
#if defined(unix) || defined(__APPLE__)
#ifdef __APPLE__
        // Workaround for Mac OS X. See:
        // http://stackoverflow.com/questions/20534788/why-does-wordexp-fail-with-wrde-syntax-on-os-x
        signal(SIGCHLD, SIG_DFL);
#endif
        // Expand string into segments
        int res = wordexp(path, &word, 0);
#ifdef __APPLE__
        signal(SIGCHLD, SIG_IGN);
#endif
        if (res != 0) {
            printf("fullPath> wordexp() failed: %d\n", res);
            return NULL;
        }

        // Get length of complete string
        lenPath = 0;
        for (unsigned int i = 0; i < word.we_wordc; i++) {
            lenPath += strlen(word.we_wordv[i]);
        }

        // Allocate new string
        dir = new char[lenPath + 2]; // space for end char

        // Copy segments into new string
        offset = 0;
        for (unsigned int i = 0; i < word.we_wordc; i++) {
            unsigned int len = strlen(word.we_wordv[i]);
            strncpy(dir + offset, word.we_wordv[i], len);
            offset += len;
        }

        wordfree(&word);
#else
        printf("WARNING: Tilde expansion not supported on this platform!\n");
        lenPath = strlen(path);
        dir = new char[lenPath + 2]; // space for end char
        strncpy(dir, path, lenPath);
#endif
    } else {
        lenPath = strlen(path);
        dir = new char[lenPath + 2]; // space for end char
        strncpy(dir, path, lenPath);
    }

    // Make sure ends in "end" char
    if ((lenPath > 0) && (end != 0) && (dir[lenPath - 1] != end)) {
        dir[lenPath] = end;
        dir[lenPath + 1] = 0;
    } else {
        dir[lenPath] = 0;
    }

    return dir;
}

bool rc_command(const char *symbol, char *command) {
    assert(symbol != NULL);
    assert(symbol[0] != '\0');
    assert(command != NULL);
    assert(command[0] != '\0');

    int lens = strlen(symbol);

    if (strncmp(command, symbol, lens) == 0) {
        int lenc = strlen(command);

        //! \fixme Should ignore whitespace, but only if it is really there...?
        // lens+1 skips '=' or ' '
        for (int i = 0, j = lens+1; j < lenc; ++i, ++j) {
            command[i] = command[j];
            command[i+1] = 0;
        }

        return true;
    }

    return false;
}

int rc_get_bool(const char *buffer, bool *val) {
    assert(buffer != NULL);
    assert(buffer[0] != '\0');
    assert(val != NULL);

    if ((buffer[0] == '1') || (strncmp(buffer, "true", 4) == 0))
        *val = true;
    else if ((buffer[0] == '0') || (strncmp(buffer, "false", 5) == 0))
        *val = false;
    else
        return -2;

    return 0;
}

