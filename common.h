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

// RSS stream download command: "RSSGET RSSTEMPFILE RSSURL"
#define RSSGET "wget -q -t 1 -T 30 -O"

#endif // __RSSREADER_COMMON_H

