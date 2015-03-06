/*
 * config.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "config.h"
#include "common.h"

cRssReaderConfig RssReaderConfig;

cRssReaderConfig::cRssReaderConfig(void)
: traceModeM(eTraceModeNormal),
  hideMenuM(0),
  hideElemM(0),
  useProxyM(0)
{
  strn0cpy(httpProxyM, "127.0.0.1:8000", sizeof(httpProxyM));
  strn0cpy(configFileM, RSSREADER_CONF, sizeof(configFileM));
}

void cRssReaderConfig::SetHttpProxy(const char *strP)
{
  strn0cpy(httpProxyM, strP, sizeof(httpProxyM));
}

void cRssReaderConfig::SetConfigFile(const char *strP)
{
  strn0cpy(configFileM, strP, sizeof(configFileM));
}
