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

char *striphtml(char *str);
void *myrealloc(void *ptr, size_t size);
bool isimage(const char *text);
bool isvideo(const char *text);
bool ismusic(const char *text);

#endif // __RSSREADER_TOOLS_H

