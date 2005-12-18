/*
 * common.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __RSSREADER_COMMON_H
#define __RSSREADER_COMMON_H

#ifdef DEBUG
#define debug(x...) dsyslog("RSSReader: " x);
#define error(x...) esyslog("RSSReader: " x);
#else
#define debug(x...) ;
#define error(x...) esyslog("RSSReader: " x);
#endif

// Temporary file used by RSSGET
#ifndef RSSTEMPFILE
#define RSSTEMPFILE "/tmp/rssreader.vdr"
#endif

// RSS stream download command
#define RSSGET "wget -O " RSSTEMPFILE " >/dev/null 2>/dev/null"

#endif // __RSSREADER_COMMON_H

