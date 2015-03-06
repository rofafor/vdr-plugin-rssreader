/*
 * config.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __RSSREADER_CONFIG_H
#define __RSSREADER_CONFIG_H

struct cRssReaderConfig
{
public:
  cRssReaderConfig(void);
  int hideMenuM;
  int hideElemM;
  int useProxyM;
  char httpProxyM[256];
  char configFileM[256];
};

extern cRssReaderConfig RssConfig;

#endif // __RSSREADER_CONFIG_H
