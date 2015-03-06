/*
 * tools.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __RSSREADER_TOOLS_H
#define __RSSREADER_TOOLS_H

#include <vdr/tools.h>

char *striphtml(char *strP);
void *myrealloc(void *ptrP, size_t sizeP);
bool isimage(const char *textP);
bool isvideo(const char *textP);
bool ismusic(const char *textP);

#endif // __RSSREADER_TOOLS_H
