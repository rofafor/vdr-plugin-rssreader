/*
 * log.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __RSSREADER_LOG_H
#define __RSSREADER_LOG_H

#include "config.h"

#define error(x...)   esyslog("RSSREADER-ERROR: " x)
#define info(x...)    isyslog("RSSREADER: " x)
// 0x0001: Generic call stack
#define debug1(x...)  void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug1)  ? dsyslog("RSSREADER1: " x)  : void() )
// 0x0002: TBD
#define debug2(x...)  void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug2)  ? dsyslog("RSSREADER2: " x)  : void() )
// 0x0004: TBD
#define debug3(x...)  void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug3)  ? dsyslog("RSSREADER3: " x)  : void() )
// 0x0008: TBD
#define debug4(x...)  void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug4)  ? dsyslog("RSSREADER4: " x)  : void() )
// 0x0010: TBD
#define debug5(x...)  void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug5)  ? dsyslog("RSSREADER5: " x)  : void() )
// 0x0020: TBD
#define debug6(x...)  void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug6)  ? dsyslog("RSSREADER6: " x)  : void() )
// 0x0040: TBD
#define debug7(x...)  void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug7)  ? dsyslog("RSSREADER7: " x)  : void() )
// 0x0080: TBD
#define debug8(x...)  void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug8)  ? dsyslog("RSSREADER8: " x)  : void() )
// 0x0100: TBD
#define debug9(x...)  void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug9)  ? dsyslog("RSSREADER9: " x)  : void() )
// 0x0200: TBD
#define debug10(x...) void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug10) ? dsyslog("RSSREADER10: " x) : void() )
// 0x0400: TBD
#define debug11(x...) void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug11) ? dsyslog("RSSREADER11: " x) : void() )
// 0x0800: TBD
#define debug12(x...) void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug12) ? dsyslog("RSSREADER12: " x) : void() )
// 0x1000: TBD
#define debug13(x...) void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug13) ? dsyslog("RSSREADER13: " x) : void() )
// 0x2000: TBD
#define debug14(x...) void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug14) ? dsyslog("RSSREADER14: " x) : void() )
// 0x4000: CURL debug file
#define debug15(x...) void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug15) ? dsyslog("RSSREADER15: " x) : void() )
// 0x8000; Extra call stack
#define debug16(x...) void( RssReaderConfig.IsTraceMode(cRssReaderConfig::eTraceModeDebug16) ? dsyslog("RSSREADER16: " x) : void() )

#endif // __RSSREADER_LOG_H
