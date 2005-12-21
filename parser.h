/*
 * parser.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __RSSREADER_PARSER_H
#define __RSSREADER_PARSER_H

#include "common.h"

// --- public functions -------------------------------------------------

int rss_parser(char * filename);
int rss_downloader(const char *str);

// --- cItem ------------------------------------------------------------

#define MAXLONGTEXTLEN  8192
#define MAXSHORTTEXTLEN 1024

class cItem : public cListObject {
private:
  char date[MAXSHORTTEXTLEN];
  char title[MAXSHORTTEXTLEN];
  char link[MAXSHORTTEXTLEN];
  char desc[MAXLONGTEXTLEN];
public:
  cItem(const char *Title, const char *Desc, const char *Date, const char *Link);
  cItem();
  void Clear(void);
  char *GetTitle(void) { return title; }
  char *GetLink(void) { return link; }
  char *GetDesc(void) { return desc; }
  char *GetDate(void) { return date; }
  void SetDate(const char *s) { debug("SetDate(): '%s'\n", s); strncpy(date, s, MAXSHORTTEXTLEN); }
  void SetTitle(const char *s) { debug("SetTitle(): '%s'\n", s); strncpy(title, s, MAXSHORTTEXTLEN); }
  void SetLink(const char *s) { debug("SetLink(): '%s'\n", s); strncpy(link, s, MAXSHORTTEXTLEN); }
  void SetDesc(const char *s) { debug("SetDesc(): '%s'\n", s); strncpy(desc, s, MAXLONGTEXTLEN); }
  void SetUTF8Date(const char *s);
  void SetUTF8Title(const char *s);
  void SetUTF8Link(const char *s);
  void SetUTF8Desc(const char *s);
};

// --- cItems -----------------------------------------------------------

class cItems : public cList<cItem> {
private:
  cItem *current;
};

extern cItems Items;

#endif // __RSSREADER_PARSER_H

