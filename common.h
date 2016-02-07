/*
 * common.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __RSSREADER_COMMON_H
#define __RSSREADER_COMMON_H

#define ERROR_IF_FUNC(exp, errstr, func, ret)                  \
  do {                                                         \
     if (exp) {                                                \
        char tmp[64];                                          \
        esyslog("[%s,%d]: " errstr ": %s", __FILE__, __LINE__, \
                strerror_r(errno, tmp, sizeof(tmp)));          \
        func;                                                  \
        ret;                                                   \
        }                                                      \
  } while (0)

#define ERROR_IF_RET(exp, errstr, ret) ERROR_IF_FUNC(exp, errstr, ,ret);

#define ERROR_IF(exp, errstr) ERROR_IF_FUNC(exp, errstr, , );

#define DELETE_POINTER(ptr)      \
  do {                           \
     if (ptr) {                  \
        typeof(*ptr) *tmp = ptr; \
        ptr = NULL;              \
        delete(tmp);             \
        }                        \
  } while (0)

#define FREE_POINTER(ptr)        \
  do {                           \
     if (ptr) {                  \
        typeof(*ptr) *tmp = ptr; \
        ptr = NULL;              \
        free(tmp);               \
        }                        \
  } while (0)

#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

extern const char VERSION[];

#endif // __RSSREADER_COMMON_H
