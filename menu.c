/*
 * menu.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
 
#include <vdr/status.h>
#include "i18n.h"
#include "parser.h"
#include "config.h"
#include "menu.h"
#include "common.h"

// --- Globals ----------------------------------------------------------

cRssItems RssItems;

// --- cRssItem ---------------------------------------------------------

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

// --- cRssItems --------------------------------------------------------

bool cRssItems::Load(const char *filename)
{
  if (cConfig<cRssItem>::Load(filename, true)) {
     return true;
     }
  return false;
}

// --- cRssMenuItem --------------------------------------------------------

cRssMenuItem::cRssMenuItem(const char *Title, const char *Date, const char *Desc, const char *Link)
:cOsdMenu(tr("RSS item"))
{
  asprintf(&text, "\n%s%s%s%s%s%s%s",
           *Date  ? strdup(Date)  : RssConfig.hideelem ? "" : tr("<no date available>"),
           (*Date  || !RssConfig.hideelem) ? "\n\n" : "",
           *Title ? strdup(Title) : RssConfig.hideelem ? "" : tr("<no title available>"),
           (*Title || !RssConfig.hideelem) ? "\n\n" : "",
           *Desc  ? strdup(Desc)  : RssConfig.hideelem ? "" : tr("<no description available>"),
           (*Desc  || !RssConfig.hideelem) ? "\n\n" : "",
           *Link  ? strdup(Link)  : RssConfig.hideelem ? "" : tr("<no link available>"));
}

cRssMenuItem::~cRssMenuItem()
{
  free(text);
}

void cRssMenuItem::Display(void)
{
  cOsdMenu::Display();
  debug("cRssMenuItem::Display(): '%s'\n", text);
  DisplayMenu()->SetText(text, true);
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
       case kOk:
            return osBack;
       default:
            state = osContinue;
       }
     state = osContinue;
     }
  return state;
}

// --- cRssItemsMenu --------------------------------------------------------

cRssItemsMenu::cRssItemsMenu(void)
:cOsdMenu(tr("Select RSS item"))
{
  for (cItem *rssItem = Parser.Items.First(); rssItem; rssItem = Parser.Items.Next(rssItem))
     Add(new cOsdItem(rssItem->GetTitle()));
  Display();
}

eOSState cRssItemsMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key) {
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
  return AddSubMenu(new cRssMenuItem(rssItem->GetTitle(), rssItem->GetDate(), rssItem->GetDesc(), rssItem->GetLink()));
}

// --- cRssStreamsMenu -----------------------------------------------------

cRssStreamsMenu::cRssStreamsMenu(void)
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
  Display();
}

eOSState cRssStreamsMenu::Select(void)
{
  cRssItem *rssItem = (cRssItem *)RssItems.Get(Current());
  if (rssItem) {
     debug("cRssStreamsMenu::Select(): downloading and parsing '%s'", rssItem->Title());
     //Skins.Message(mtInfo, tr("Loading RSS stream...")); // this message generates annoying slowdown 
     if (Parser.Download(rssItem->Url())) {
        if (Parser.Parse(RssConfig.tempfile)) {
           return AddSubMenu(new cRssItemsMenu);
           }
        else {
           Skins.Message(mtError, tr("Can't parse RSS stream!"));
           return osContinue;
           }
        }
     else {
        Skins.Message(mtError, tr("Can't download RSS stream!"));
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
       case kOk:
            return Select();
       default:
            break;
       }
     state = osContinue;
     }
  return state;
}
