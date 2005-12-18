/*
 * parser.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __RSSREADER_PARSER_H
#define __RSSREADER_PARSER_H

#include <vdr/tools.h>
#include <vdr/interface.h>
#include "common.h"

// --- public functions -------------------------------------------------

int rss_parser(char * filename);
int rss_downloader(const char *str);

// --- cItem ------------------------------------------------------------

#define MAXLONGTEXTLEN  2048
#define MAXSHORTTEXTLEN 256

class cItem : public cListObject {
private:
  char date[MAXSHORTTEXTLEN];
  char title[MAXSHORTTEXTLEN];
  char desc[MAXLONGTEXTLEN];
  char target[MAXLONGTEXTLEN];
public:
  cItem(const char *Title, const char *Desc, const char *Date, const char *Target);
  cItem();
  void Clear(void);
  char *GetTitle(void) { return title; }
  char *GetDesc(void) { return desc; }
  char *GetDate(void) { return date; }
  char *GetTarget(void) { return target; }
  void SetDate(const char *s) { debug("SetDate(): '%s'\n", s); strncpy(date, s, MAXSHORTTEXTLEN); }
  void SetTitle(const char *s) { debug("SetTitle(): '%s'\n", s); strncpy(title, s, MAXSHORTTEXTLEN); }
  void SetDesc(const char *s) { debug("SetDesc(): '%s'\n", s); strncpy(desc, s, MAXLONGTEXTLEN); }
  void SetTarget(const char *s) { debug("SetTarget(): '%s'\n", s); strncpy(target, s, MAXLONGTEXTLEN); }
  void SetUTF8Date(const char *s);
  void SetUTF8Title(const char *s);
  void SetUTF8Desc(const char *s);
  void SetUTF8Target(const char *s);
};

// --- cItems -----------------------------------------------------------

class cItems : public cList<cItem> {
private:
  cItem *current;
};

extern cItems Items;

#endif // __RSSREADER_PARSER_H

