/*
 * tools.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __RSSREADER_TOOLS_H
#define __RSSREADER_TOOLS_H

#include <vdr/tools.h>

int   charsetconv(const char *buffer, int buf_len, const char *str, int str_len, const char *from, const char *to);
char *htmlcharconv(char *str);
char *striphtml(char *str);
void *myrealloc(void *ptr, size_t size);

#endif // __RSSREADER_TOOLS_H

