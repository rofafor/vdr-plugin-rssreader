/*
 * parser.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __RSSREADER_PARSER_H
#define __RSSREADER_PARSER_H

#include "common.h"

// --- Defines ----------------------------------------------------------------

#define SHORT_TEXT_LEN 2048
#define LONG_TEXT_LEN  16384

// --- cRssParserItem(s) ------------------------------------------------------

class cRssParserItem : public cListObject {
private:
  cCharSetConv convM;
  char dateM[SHORT_TEXT_LEN];
  char titleM[SHORT_TEXT_LEN];
  char linkM[SHORT_TEXT_LEN];
  char descriptionM[LONG_TEXT_LEN];

public:
  cRssParserItem();
  ~cRssParserItem();
  void Clear(void);
  char *GetDate(void) { return dateM; }
  char *GetTitle(void) { return titleM; }
  char *GetLink(void) { return linkM; }
  char *GetDescription(void)  { return descriptionM; }
  void SetDate(const char *strP);
  void SetTitle(const char *strP);
  void SetLink(const char *strP);
  void SetDescription(const char *strP);
};

class cRssParserItems : public cList<cRssParserItem> {
};

// --- cRssParser -------------------------------------------------------------

struct MemoryStruct {
  char   *memory;
  size_t size;
};

class cRssParser {
private:
  struct MemoryStruct dataM;
  cRssParserItems itemsM;
  void ResetMemory(void);
public:
  enum eRssError {
    RSS_PARSING_OK     =  0,
    RSS_UNKNOWN_ERROR  = -1,
    RSS_DOWNLOAD_ERROR = -2,
    RSS_PARSING_ERROR  = -3
  };
  cRssParser();
  ~cRssParser();
  int DownloadAndParse(const char *urlP);
  cRssParserItems *Items(void) { return &itemsM; }
  };

extern cRssParser RssParser;

#endif // __RSSREADER_PARSER_H
