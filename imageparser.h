#ifndef IMAGEPARSER_H
#define IMAGEPARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <curl/curl.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>

struct ParseItemData {
    std::string title;
    std::string link;
    std::string imageUrl;
};

static size_t writer(char *data, size_t size, size_t nmemb, void *userp);

std::string generateUniqueFilename(const std::string& original_url);

std::string downloadAndSaveImage(const std::string& image_url_to_download);

std::string extractOgImageUrl(std::string url);

void* parseSingleRssItemThread(void* arg);

std::vector<std::vector<std::string>> parseRSS(std::string url);

#endif 
