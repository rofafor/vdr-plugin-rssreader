/*
 * parser.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <stack> 
#include <expat.h>
#include <iconv.h>
#include <vdr/config.h>
#include <vdr/i18n.h>
#include "config.h"
#include "parser.h"
#include "common.h"

// --- static functions -------------------------------------------------

static int charsetconv(const char *buffer, int buf_len, const char *str, int str_len, const char *from, const char *to)
{
  if (to && from) {
     iconv_t ic = iconv_open(to, from);
     if (ic >= 0) {
        size_t inbytesleft = str_len;
        size_t outbytesleft = buf_len;
        char *out = (char*)buffer;
        int ret;
        if ((ret = iconv(ic, (char**)&str, &inbytesleft, &out, &outbytesleft)) >= 0) {
           iconv_close(ic);
           return buf_len - outbytesleft;
           }
        iconv_close(ic);
        }
     }
  else {
     error("charsetconv(): charset is not valid");
     }
  return -1;
}

static char *striphtml(char *s)
{
  char *c, t = 0, *r;
  c = s;
  r = s;
  while (*s != '\0') {
    if (*s == '<')
       t++;
    else if (*s == '>')
       t--;
    else if (t < 1)
       *(c++) = *s;
    s++;
    }
  *c = '\0';
  return r;
}

static char *stripspaces(char *str)
{
  char tmp[MAXLONGTEXTLEN];
  char *ptr;

  if (str == NULL)
     return str;

  strncpy(tmp, str, MAXLONGTEXTLEN);
  strcpy(str, "");
  ptr = strtok(tmp, " \n\t\r\x3F");
  while (ptr) {
     strcat(str, ptr);
     strcat(str, " ");
     ptr = strtok(NULL, " \n\t\r\x3F");
     }
  return str;
}

// --- cItem ------------------------------------------------------------

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

// --- Expat ------------------------------------------------------------

#define BUFFSIZE 16384

cItems  Items;

cItem   *item;
int     depth;
char    data_string[MAXLONGTEXTLEN];
char    buff[BUFFSIZE];

struct XmlNode
{
      char  nodename[MAXSHORTTEXTLEN];
      int   depth;
};

std::stack<struct XmlNode> nodestack;

static int XMLCALL
unknownencoding(void *data,const XML_Char *encoding, XML_Encoding *info)
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

static void XMLCALL
start(void *data, const char *el, const char **attr)
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

static void XMLCALL
end(void *data, const char *el)
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
        Items.Add(item);
     }
  else if (!strncmp(el, "title", 5)) {
     stripspaces(data_string);
     if (!strncmp(parent, "item", 4)) {
        debug("depth: %d", depth);
        item->SetUTF8Title(data_string);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("rss_parser(): RSS title '%s'", data_string);
        }
     }
  else if (!strncmp(el, "link", 4)) {
     stripspaces(data_string);
     if (!strncmp(parent, "item", 4)) {
        item->SetUTF8Link(data_string);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("rss_parser(): RSS link '%s'", data_string);
        }
     }
  else if (!strncmp(el, "pubDate", 7)) {
     stripspaces(data_string);
     if (!strncmp(parent, "item", 4)) {
        item->SetUTF8Date(data_string);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("rss_parser(): RSS date '%s'", data_string);
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
        debug("rss_parser(): RSS description '%s'", data_string);
        }
     }
  strcpy(data_string, "");
}

static void
data(void *user_data, const XML_Char *s, int len)
{
  /* Only until the maximum size of the buffer */
  if (strlen(data_string) + len <= MAXLONGTEXTLEN)
     strncat(data_string, s, len);
}

// --- Parse RSS  -------------------------------------------------------

int rss_parser(char * filename)
{
  FILE *fp;

  depth = 0;
  // Setup expat
  XML_Parser p = XML_ParserCreate(NULL);
  if (!p) {
     error("rss_parser(): couldn't allocate memory for parser");
     return 0;
     }

  XML_SetElementHandler(p, start, end);
  XML_SetCharacterDataHandler(p, data);
  XML_SetUnknownEncodingHandler(p, unknownencoding, NULL);

  // Clear Items list to play
  Items.Clear();
  // Load the RSS
  if ((fp = fopen(filename, "r")) == NULL) {
     error("rss_parser(): file does not exist");
     return 0;
     }

  // Go through all items
  for (;;) {
     int done;
     int len;

     len = fread(buff, 1, BUFFSIZE, fp);
     if (ferror(fp)) {
        error("rss_parser(): Read error");
        return 0;
        }
     done = feof(fp);
     if (XML_Parse(p, buff, len, done) == XML_STATUS_ERROR) {
        error("rss_parser(): Parse error at line %d:\n%s\n", XML_GetCurrentLineNumber(p), XML_ErrorString(XML_GetErrorCode(p)));
        return 0;
        }

     if (done)
        break;
     }
  return -1;
}

int rss_downloader(const char *str)
{
  char *cmd;

  asprintf(&cmd, "%s %s '%s'", RSSGET, RssConfig.tempfile, str);
  debug("rss_downloader(): running '%s'", cmd);
  int r = system(cmd);
  if (r != 0) {
     error("rss_downloader(): page download (via '%s') failed", cmd);
     free(cmd);
     return 0;
     }
  free(cmd);
  debug("rss_downloader(): done (return code: %d)", r);
  return -1;
}

