/*
 * parser.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __RSSREADER_PARSER_H
#define __RSSREADER_PARSER_H

#include "common.h"

// --- Defines ----------------------------------------------------------

#define SHORT_TEXT_LEN 2048
#define LONG_TEXT_LEN  16384

// --- cItem(s) ---------------------------------------------------------

class cItem : public cListObject {
private:
  cCharSetConv convM;
  char dateM[SHORT_TEXT_LEN];
  char titleM[SHORT_TEXT_LEN];
  char linkM[SHORT_TEXT_LEN];
  char descriptionM[LONG_TEXT_LEN];
public:
  cItem();
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

class cItems : public cList<cItem> {
};

// --- cParser ----------------------------------------------------------

struct MemoryStruct {
  char   *memory;
  size_t size;
};

class cParser {
private:
  struct MemoryStruct dataM;
  void ResetMemory(void);
public:
  enum eRssError {
    RSS_PARSING_OK     =  0,
    RSS_UNKNOWN_ERROR  = -1,
    RSS_DOWNLOAD_ERROR = -2,
    RSS_PARSING_ERROR  = -3
  };
  cParser();
  ~cParser();
  int DownloadAndParse(const char *urlP);
  cItems Items;
  };

extern cParser Parser;

#endif // __RSSREADER_PARSER_H
