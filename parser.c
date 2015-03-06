/*
 * parser.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <stack>
#include <curl/curl.h>
#include <curl/easy.h>
#include <vdr/config.h>
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

cItem::cItem()
: convM("UTF-8", cCharSetConv::SystemCharacterTable())
{
  strcpy(dateM, "");
  strcpy(titleM, "");
  strcpy(linkM, "");
  strcpy(descriptionM, "");
}


void cItem::Clear(void)
{
  strcpy(dateM, "");
  strcpy(titleM, "");
  strcpy(linkM, "");
  strcpy(descriptionM, "");
}

void cItem::SetDate(const char *strP)
{
  Utf8Strn0Cpy(dateM, strP, sizeof(dateM));
  compactspace(dateM);
  Utf8Strn0Cpy(dateM, convM.Convert(dateM), sizeof(dateM));
}

void cItem::SetTitle(const char *strP)
{
  Utf8Strn0Cpy(titleM, strP, sizeof(titleM));
  compactspace(titleM);
  striphtml(titleM);
  Utf8Strn0Cpy(titleM, convM.Convert(titleM), sizeof(titleM));
}

void cItem::SetLink(const char *strP)
{
  Utf8Strn0Cpy(linkM, strP, sizeof(linkM));
  compactspace(linkM);
  Utf8Strn0Cpy(linkM, convM.Convert(linkM), sizeof(linkM));
}

void cItem::SetDescription(const char *strP)
{
  Utf8Strn0Cpy(descriptionM, strP, sizeof(descriptionM));
  compactspace(descriptionM);
  striphtml(descriptionM);
  Utf8Strn0Cpy(descriptionM, convM.Convert(descriptionM), sizeof(descriptionM));
}

// --- Parse RSS  -------------------------------------------------------

struct XmlNode {
  char nodename[SHORT_TEXT_LEN];
  int  depth;
};

cItem *itemS = NULL;
int depthS = 0;
char dataStringS[LONG_TEXT_LEN];
std::stack<struct XmlNode> nodeStackS;

// Character set mappings can be found at:
// ftp://ftp.unicode.org/Public/MAPPINGS/

// ISO-8859-15
static int MapLatin9S[0x80] = {
      -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
      -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
      -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
      -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
  0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x20ac, 0x00a5, 0x0160, 0x00a7,
  0x0161, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af,
  0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x017d, 0x00b5, 0x00b6, 0x00b7,
  0x017e, 0x00b9, 0x00ba, 0x00bb, 0x0152, 0x0153, 0x0178, 0x00bf,
  0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7,
  0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
  0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7,
  0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df,
  0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
  0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
  0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
  0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff
};

// ISO-8859-2
static int MapLatin2S[0x80] = {
      -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
      -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
      -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
      -1,     -1,     -1,     -1,     -1,     -1,     -1,     -1,
  0x00a0, 0x0104, 0x02d8, 0x0141, 0x00a4, 0x013d, 0x015a, 0x00a7,
  0x00a8, 0x0160, 0x015e, 0x0164, 0x0179, 0x00ad, 0x017d, 0x017b,
  0x00b0, 0x0105, 0x02db, 0x0142, 0x00b4, 0x013e, 0x015b, 0x02c7,
  0x00b8, 0x0161, 0x015f, 0x0165, 0x017a, 0x02dd, 0x017e, 0x017c,
  0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7,
  0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e,
  0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7,
  0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df,
  0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7,
  0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f,
  0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7,
  0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9
};

// Windows CP1250
static int MapCp1250S[0x80] = {
  0x20ac,     -1, 0x201a,     -1, 0x201e, 0x2026, 0x2020, 0x2021,
      -1, 0x2030, 0x0160, 0x2039, 0x015a, 0x0164, 0x017d, 0x0179,
      -1, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
      -1, 0x2122, 0x0161, 0x203a, 0x015b, 0x0165, 0x017e, 0x017a,
  0x00a0, 0x02c7, 0x02d8, 0x0141, 0x00a4, 0x0104, 0x00a6, 0x00a7,
  0x00a8, 0x00a9, 0x015e, 0x00ab,     -1, 0x00ad, 0x00ae, 0x017b,
  0x00b0, 0x00b1, 0x02db, 0x0142, 0x00b4, 0x00b5, 0x00b6, 0x00b7,
  0x00b8, 0x0105, 0x015f, 0x00bb, 0x013d, 0x02dd, 0x013e, 0x017c,
  0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7,
  0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e,
  0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7,
  0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df,
  0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7,
  0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f,
  0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7,
  0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9
};

static int XMLCALL UnknownEncodingHandler(void *dataP, const XML_Char *encodingP, XML_Encoding *infoP)
{
  int *map = NULL;

  if (strcmp(encodingP, "iso-8859-15") == 0) {
     map = MapLatin9S;
     }
  if (strcmp(encodingP, "iso-8859-2") == 0) {
     map = MapLatin2S;
     }
  else if (strcmp(encodingP, "windows-1250") == 0) {
     map = MapCp1250S;
     }

  if (map) {
     int i;
     for (i = 0; i < 0x80; ++i) {
         infoP->map[i] = i;
         infoP->map[i + 0x80] = map[i];
         }
     infoP->map[0x7f] = -1;
     infoP->data      = NULL;
     infoP->convert   = NULL;
     infoP->release   = NULL;
     return XML_STATUS_OK;
     }

  return XML_STATUS_ERROR;
}

static inline bool IsItemTag(const char *tagP)
{
  if (!strncmp(tagP, "item", 4) || !strncmp(tagP, "entry", 5))
     return true;
  return false;
}

static inline bool IsTitleTag(const char *tagP)
{
  if (!strncmp(tagP, "title", 5))
     return true;
  return false;
}

static inline bool IsLinkTag(const char *tagP)
{
  if (!strncmp(tagP, "link", 4))
     return true;
  return false;
}

static inline bool IsDateTag(const char *tagP)
{
  if (!strncmp(tagP, "pub", 3))
     return true;
  return false;
}

static inline bool IsDescriptionTag(const char *tagP)
{
  if (!strncmp(tagP, "description", 11) || !strncmp(tagP, "content", 7))
     return true;
  return false;
}

static void XMLCALL StartHandler(void *dataP, const char *elemP, const char **attrP)
{
  XmlNode node;

  strn0cpy(node.nodename, elemP, sizeof(node.nodename));
  node.depth = depthS;
  nodeStackS.push(node);

  if (IsItemTag(elemP)) {
     cItem *tmpitem = new cItem;
     itemS = tmpitem;
     itemS->Clear();
     }
  strcpy(dataStringS, "");
  depthS++;
}

static void XMLCALL EndHandler(void *dataP, const char *elemP)
{
  nodeStackS.pop();
  if (!nodeStackS.empty()) {
     char parent[SHORT_TEXT_LEN];

     strn0cpy(parent, (nodeStackS.top()).nodename, sizeof((nodeStackS.top()).nodename));
     // No need to free the node
     if (IsItemTag(elemP) && itemS && *itemS->GetTitle())
        Parser.Items.Add(itemS); // End of the current item
     else if (IsTitleTag(elemP) && IsItemTag(parent))
        itemS->SetTitle(dataStringS);
     else if (IsLinkTag(elemP) && IsItemTag(parent))
        itemS->SetLink(dataStringS);
     else if (IsDateTag(elemP) && IsItemTag(parent))
        itemS->SetDate(dataStringS);
     else if (IsDescriptionTag(elemP) && IsItemTag(parent))
        itemS->SetDescription(dataStringS);
     }
  --depthS;
}

static void DataHandler(void *dataP, const XML_Char *strP, int lenP)
{
  // Only until the maximum size of the buffer
  if ((strlen(dataStringS) + lenP) <= LONG_TEXT_LEN)
     strncat(dataStringS, strP, lenP);
}

static size_t WriteMemoryCallback(void *ptrP, size_t sizeP, size_t nmembP, void *dataP)
{
  size_t realsize = sizeP * nmembP;
  struct MemoryStruct *mem = (struct MemoryStruct *)dataP;

  mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory) {
     memcpy(&(mem->memory[mem->size]), ptrP, realsize);
     mem->size += realsize;
     mem->memory[mem->size] = 0;
     }
  return realsize;
}

cParser::cParser()
: Items()
{
  dataM.memory = NULL;
  dataM.size = 0;
}

cParser::~cParser()
{
  ResetMemory();
}

void cParser::ResetMemory(void)
{
  // free any allocated memory
  FREE_POINTER(dataM.memory);
  dataM.size = 0;
}

int cParser::DownloadAndParse(const char *urlP)
{
  CURL *curl_handle;

  // Clear Items list and initialize depth
  Items.Clear();
  depthS = 0;
  ResetMemory();

  // Init the curl session
  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();

  // Specify URL to get
  curl_easy_setopt(curl_handle, CURLOPT_URL, urlP);

  // Specify HTTP proxy: my.proxy.com:80
  if (RssConfig.useProxyM) {
     curl_easy_setopt(curl_handle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
     curl_easy_setopt(curl_handle, CURLOPT_PROXY, RssConfig.httpProxyM);
     }

  // Send all data to this function
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  // Set maximum file size to get (bytes)
  curl_easy_setopt(curl_handle, CURLOPT_MAXFILESIZE, 1048576L);

  // No progress meter
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

  // No signaling
  curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1L);

  // Follow location
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

  // Set timeout to 15 seconds
  curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 15L);

  // Pass our 'data' struct to the callback function
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&dataM);

  // Some servers don't like requests that are made without a user-agent field
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, RSSREADER_USERAGENT);

  // Get it!
  if (curl_easy_perform(curl_handle) != 0) {
     // Cleanup curl stuff
     curl_easy_cleanup(curl_handle);
     ResetMemory();
     error("cParser::DownloadAndParse(): couldn't download the stream");
     return (RSS_DOWNLOAD_ERROR);
     }

  if (dataM.size) {
#ifdef DEBUG
     // Only for debug dump
     FILE *fp = fopen("/tmp/rssreader.vdr", "w");
     if (fp) {
        if (fwrite(dataM.memory, 1, dataM.size, fp) != dataM.size)
           error("cParser::DownloadAndParse(): couldn't write debug dump");
        fclose(fp);
        }
#endif
     // Setup expat
     XML_Parser p = XML_ParserCreate(NULL);
     if (!p) {
        // Cleanup curl stuff
        curl_easy_cleanup(curl_handle);
        ResetMemory();
        error("cParser::DownloadAndParse(): couldn't allocate memory for parser");
        return (RSS_UNKNOWN_ERROR);
        }
     XML_SetElementHandler(p, StartHandler, EndHandler);
     XML_SetCharacterDataHandler(p, DataHandler);
     XML_SetUnknownEncodingHandler(p, UnknownEncodingHandler, NULL);

     if (XML_Parse(p, dataM.memory, (int)dataM.size, XML_TRUE) == XML_STATUS_ERROR) {
        // Cleanup curl stuff
        curl_easy_cleanup(curl_handle);
        ResetMemory();
        error("cParser::DownloadAndParse(): Parse error at line %ld:\n%s\n", XML_GetCurrentLineNumber(p), XML_ErrorString(XML_GetErrorCode(p)));
        return (RSS_PARSING_ERROR);
        }
     }

  // Cleanup curl stuff
  curl_easy_cleanup(curl_handle);
  ResetMemory();

  return (RSS_PARSING_OK);
}
