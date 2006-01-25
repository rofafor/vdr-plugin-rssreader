/*
 * parser.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <stack>
#include <vdr/config.h>
#include <vdr/i18n.h>
#include "common.h"
#include "config.h"
#include "tools.h"
#include "parser.h"

#include <expat.h>
#define LIBEXPAT_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#if LIBEXPAT_VERSION(XML_MAJOR_VERSION, XML_MINOR_VERSION, XML_MICRO_VERSION) < LIBEXPAT_VERSION(1, 95, 8)
#warning "Expat XML parser library 1.95.8 or newer required!"
#endif

// --- Globals ----------------------------------------------------------

cParser Parser;

// --- cItem(s) ---------------------------------------------------------

cItem::cItem(const char *Title, const char *Desc, const char *Date, const char *Link)
{
  strncpy(date, Date, sizeof(date));
  strncpy(title, Title, sizeof(title));
  strncpy(link, Link, sizeof(link));
  strncpy(desc, Desc, sizeof(desc));
}

cItem::cItem(void)
{
  strcpy(date, "");
  strcpy(title, "");
  strcpy(link, "");
  strcpy(desc, "");
}

void cItem::Clear(void)
{
  strcpy(date, "");
  strcpy(title, "");
  strcpy(link, "");
  strcpy(desc, "");
}

void cItem::SetUTF8Date(const char *s)
{
  char tmp[MAXSHORTTEXTLEN];
  memset(tmp, 0, sizeof(tmp));

  charsetconv(tmp, sizeof(tmp), s, strlen(s), "UTF8", I18nCharSets()[Setup.OSDLanguage]);
  debug("cItem::SetUTF8Date(): Date: '%s'", tmp);
  strncpy(date, tmp, sizeof(tmp));
}

void cItem::SetUTF8Title(const char *s)
{
  char tmp[MAXSHORTTEXTLEN];
  memset(tmp, 0, sizeof(tmp));

  charsetconv(tmp, sizeof(tmp), s, strlen(s), "UTF8", I18nCharSets()[Setup.OSDLanguage]);
  debug("cItem::SetUTF8Title(): '%s'", tmp);
  strncpy(title, tmp, sizeof(tmp));
}

void cItem::SetUTF8Link(const char *s)
{
  char tmp[MAXSHORTTEXTLEN];
  memset(tmp, 0, sizeof(tmp));

  charsetconv(tmp, sizeof(tmp), s, strlen(s), "UTF8", I18nCharSets()[Setup.OSDLanguage]);
  debug("cItem::SetUTF8Link(): '%s'", tmp);
  strncpy(link, tmp, sizeof(tmp));
}

void cItem::SetUTF8Desc(const char *s)
{
  char tmp[MAXLONGTEXTLEN];
  memset(tmp, 0, sizeof(tmp));

  charsetconv(tmp, sizeof(tmp), s, strlen(s), "UTF8", I18nCharSets()[Setup.OSDLanguage]);
  debug("cItem::SetUTF8Desc(): '%s'", tmp);
  strncpy(desc, tmp, sizeof(tmp));
}

// --- Parse RSS  -------------------------------------------------------

#define XMLBUFSIZE 16384

struct XmlNode {
  char nodename[MAXSHORTTEXTLEN];
  int  depth;
};

cItem *item;
int   depth;
char  buff[XMLBUFSIZE];
char  data_string[MAXLONGTEXTLEN];
std::stack<struct XmlNode> nodestack;

static int XMLCALL unknownencoding(void *data,const XML_Char *encoding, XML_Encoding *info)
{
  if (strcmp(encoding, "iso-8859-15") == 0) {
     int i;
     for (i = 0; i < 256; ++i)
        info->map[i] = i;
     info->data = NULL;
     info->convert = NULL;
     info->release = NULL;
     return XML_STATUS_OK;
     }
  return XML_STATUS_ERROR;
}

static void XMLCALL start(void *data, const char *el, const char **attr)
{
  XmlNode node;

  strncpy(node.nodename, el, MAXSHORTTEXTLEN);
  node.depth = depth;
  nodestack.push(node);

  if (!strncmp(el, "item", 4)) {
     cItem *tmpitem = new cItem;
     item = tmpitem;
     item->Clear();
     }
  depth++;
}

static void XMLCALL end(void *data, const char *el)
{
  char txt[MAXLONGTEXTLEN];
  char parent[MAXSHORTTEXTLEN];
  
  if (nodestack.size() > 1) {
     nodestack.pop();
     } 
  else {
     nodestack.pop();
     return;
     }
  strncpy(parent, (nodestack.top()).nodename, MAXSHORTTEXTLEN);
  // No need to free the node
  
  depth--;
  if (!strncmp(el, "item", 4)) {
     // End of the current item
     if (*item->GetTitle())
        Parser.Items.Add(item);
     }
  else if (!strncmp(el, "title", 5)) {
     stripspaces(data_string);
     if (!strncmp(parent, "item", 4)) {
        debug("depth: %d", depth);
        item->SetUTF8Title(data_string);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("cParser::end(): RSS title '%s'", data_string);
        }
     }
  else if (!strncmp(el, "link", 4)) {
     stripspaces(data_string);
     if (!strncmp(parent, "item", 4)) {
        item->SetUTF8Link(data_string);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("cParser::end(): RSS link '%s'", data_string);
        }
     }
  else if (!strncmp(el, "pubDate", 7)) {
     stripspaces(data_string);
     if (!strncmp(parent, "item", 4)) {
        item->SetUTF8Date(data_string);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("cParser::end(): RSS date '%s'", data_string);
        }
     }
  else if (!strncmp(el, "description", 11)) {
     if (!strncmp(parent, "item", 4)) {
        strncpy(txt, data_string, MAXLONGTEXTLEN);
        striphtml(txt);
        stripspaces(txt);
        item->SetUTF8Desc(txt);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("cParser::end(): RSS description '%s'", data_string);
        }
     }
  strcpy(data_string, "");
}

static void data(void *user_data, const XML_Char *s, int len)
{
  /* Only until the maximum size of the buffer */
  if (strlen(data_string) + len <= MAXLONGTEXTLEN)
     strncat(data_string, s, len);
}

bool cParser::Parse(char *filename)
{
  FILE *fp;

  depth = 0;
  // Setup expat
  XML_Parser p = XML_ParserCreate(NULL);
  if (!p) {
     error("cParser::Parse(): couldn't allocate memory for parser");
     return false;
     }

  XML_SetElementHandler(p, start, end);
  XML_SetCharacterDataHandler(p, data);
  XML_SetUnknownEncodingHandler(p, unknownencoding, NULL);

  // Clear Items list to play
  Items.Clear();
  // Load the RSS
  if ((fp = fopen(filename, "r")) == NULL) {
     error("cParser::Parse(): file does not exist");
     return false;
     }

  // Go through all items
  for (;;) {
     int done;
     int len;

     len = fread(buff, 1, XMLBUFSIZE, fp);
     if (ferror(fp)) {
        error("cParser::Parse(): Read error");
        return false;
        }
     done = feof(fp);
     if (XML_Parse(p, buff, len, done) == XML_STATUS_ERROR) {
        error("cParser::Parse(): Parse error at line %d:\n%s\n", XML_GetCurrentLineNumber(p), XML_ErrorString(XML_GetErrorCode(p)));
        return false;
        }

     if (done)
        break;
     }
  return true;
}

bool cParser::Download(const char *url)
{
  char *cmd;

  asprintf(&cmd, "%s %s '%s'", RSSGET, RssConfig.tempfile, url);
  debug("cParser::Download(): running '%s'", cmd);
  int r = system(cmd);
  if (r != 0) {
     error("cParser::Download(): page download (via '%s') failed (return code: %d)", cmd, r);
     free(cmd);
     return false;
     }
  free(cmd);
  debug("cParser::Download(): done (return code: %d)", r);
  return true;
}
