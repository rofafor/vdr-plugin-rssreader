/*
 * menu.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */
 
#include <vdr/plugin.h>
#include <vdr/status.h>
#include "parser.h"
#include "config.h"
#include "menu.h"
#include "tools.h"
#include "common.h"

// --- Globals ----------------------------------------------------------

cRssItems RssItems;

// --- cRssItem(s) ------------------------------------------------------

cRssItem::cRssItem(void)
{ 
  title = url = NULL;
}

cRssItem::~cRssItem()
{
  free(title);
  free(url);
}

bool cRssItem::Parse(const char *s)
{
  const char *p = strchr(s, ':');
  if (p) {
     int l = p - s;
     if (l > 0) {
        title = MALLOC(char, l + 1);
        stripspace(strn0cpy(title, s, l + 1));
        if (!isempty(title)) {
           url = stripspace(strdup(skipspace(p + 1)));
           return true;
           }
        }
     }
  return false;
}

bool cRssItems::Load(const char *filename)
{
  if (cConfig<cRssItem>::Load(filename, true)) {
     return true;
     }
  return false;
}

// --- cRssMenuItem --------------------------------------------------------

cRssMenuItem::cRssMenuItem(const char *Date, const char *Title, const char *Link, const char *Description)
:cOsdMenu(tr("RSS item"))
{
  text = cString::sprintf("\n%s%s%s%s%s%s%s",
           *Date         ? strdup(Date)          : RssConfig.hideelem ? "" : tr("<no date available>"),
           (*Date        || !RssConfig.hideelem) ? "\n\n" : "",
           *Title        ? strdup(Title)         : RssConfig.hideelem ? "" : tr("<no title available>"),
           (*Title       || !RssConfig.hideelem) ? "\n\n" : "",
           *Description  ? strdup(Description)   : RssConfig.hideelem ? "" : tr("<no description available>"),
           (*Description || !RssConfig.hideelem) ? "\n\n" : "",
           *Link         ? strdup(Link)          : RssConfig.hideelem ? "" : tr("<no link available>"));
  link = cString(Link);
  if (isimage(*link))
     type = TYPE_IMAGE;
  else if (ismusic(*link))
     type = TYPE_MUSIC;
  else if (isvideo(*link))
     type = TYPE_VIDEO;
  else
     type = TYPE_NONE;
  SetHelp(NULL, "<<", (type != TYPE_NONE) ? ">>" : NULL, NULL);
}

cRssMenuItem::~cRssMenuItem()
{
}

void cRssMenuItem::Display(void)
{
  cOsdMenu::Display();
  debug("cRssMenuItem::Display(): '%s'\n", *text);
  DisplayMenu()->SetText(text, false);
  cStatus::MsgOsdTextItem(text);
}

eOSState cRssMenuItem::ProcessKey(eKeys Key)
{
  switch (Key) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
         DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
         cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp);
         return osContinue;
    default:
         break;
    }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kGreen:
       case kOk:
            return osBack;
       case kYellow:
            switch (type) {
               case TYPE_IMAGE:
                    cPluginManager::CallFirstService("ImagePlayer-1.0", (void *)*link);
                    break;
               case TYPE_MUSIC:
                    cPluginManager::CallFirstService("MusicPlayer-1.0", (void *)*link);
                    break;
               case TYPE_VIDEO:
                    cPluginManager::CallFirstService("MediaPlayer-1.0", (void *)*link);
                    break;
               case TYPE_NONE:
               default:
                    break;
              }
       default:
            state = osContinue;
       }
     state = osContinue;
     }
  return state;
}

// --- cRssItemsMenu --------------------------------------------------------

cRssItemsMenu::cRssItemsMenu()
:cOsdMenu(tr("Select RSS item"))
{
  for (cItem *rssItem = Parser.Items.First(); rssItem; rssItem = Parser.Items.Next(rssItem))
     Add(new cOsdItem(rssItem->GetTitle()));
  SetHelp(NULL, "<<", ">>", NULL);
  Display();
}

eOSState cRssItemsMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key) {
       case kGreen:
            return osBack;
       case kYellow:
       case kOk:
            return ShowDetails();
       default:
            break;
       }
     state = osContinue;
     }
  return state;
}

eOSState cRssItemsMenu::ShowDetails(void)
{
  cItem *rssItem = (cItem *)Parser.Items.Get(Current());
  if (rssItem) {
     return AddSubMenu(new cRssMenuItem(rssItem->GetDate(), rssItem->GetTitle(), rssItem->GetLink(), rssItem->GetDescription()));
     }
  return osContinue;
}

// --- cRssStreamsMenu -----------------------------------------------------

cRssStreamsMenu::cRssStreamsMenu()
:cOsdMenu(tr("Select RSS stream"))
{
  for (cRssItem *rssItem = RssItems.First(); rssItem; rssItem = RssItems.Next(rssItem)) {
      cOsdItem *osdItem = new cOsdItem;
      if (!*rssItem->Url())
         osdItem->SetSelectable(false);
      osdItem->SetText(rssItem->Title());
      debug("StreamsMenu: '%s' : '%s'", rssItem->Title(), rssItem->Url());
      Add(osdItem);
    }
  SetHelp(NULL, "<<", ">>", NULL);
  Display();
}

eOSState cRssStreamsMenu::Select(void)
{
  cRssItem *rssItem = (cRssItem *)RssItems.Get(Current());
  if (rssItem) {
     debug("cRssStreamsMenu::Select(): downloading and parsing '%s'", rssItem->Title());
     // the following message generates an annoying slowdown 
     //Skins.Message(mtInfo, tr("Loading RSS stream..."));
     switch (Parser.DownloadAndParse(rssItem->Url())) {
       case (cParser::RSS_PARSING_OK):
            return AddSubMenu(new cRssItemsMenu);
       case (cParser::RSS_PARSING_ERROR):
            Skins.Message(mtError, tr("Can't parse RSS stream!"));
            return osContinue;
       case (cParser::RSS_DOWNLOAD_ERROR):
            Skins.Message(mtError, tr("Can't download RSS stream!"));
            return osContinue;
       case (cParser::RSS_UNKNOWN_ERROR):
       default:
            Skins.Message(mtError, tr("Unknown error!"));
            return osContinue;
       }
     }
  return osEnd;
}

eOSState cRssStreamsMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key) {
       case kGreen:
            return osBack;
       case kYellow:
       case kOk:
            return Select();
       default:
            break;
       }
     state = osContinue;
     }
  return state;
}
