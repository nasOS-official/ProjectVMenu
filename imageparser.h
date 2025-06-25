#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <curl/curl.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include <vector>

#ifndef IMAGEPARSER_H
#define IMAGEPARSER_H



#ifdef _WIN32
#define COMPARE(a, b) (!_stricmp((a), (b)))
#else
#define COMPARE(a, b) (!strcasecmp((a), (b)))
#endif


struct Context
{
    Context(): addTitle(false) { }

    bool addTitle;
    std::string title;
};

static std::string g_curl_write_buffer;
static char g_curl_error_buffer[CURL_ERROR_SIZE];


static size_t writer(char *data, size_t size, size_t nmemb,
                     std::string *writerData)
{
    if(writerData == NULL)
        return 0;

    writerData->append(data, size*nmemb);

    return size * nmemb;
}



std::string parseImage(std::string url) {
    std::string imageUrl = "";
    CURL *conn = NULL;
    CURLcode code;

    conn = curl_easy_init();
    if (conn == NULL) {
        fprintf(stderr, "parseImage: Failed to create CURL connection\n");
        return imageUrl;
    }

    code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, g_curl_error_buffer);
    if (code != CURLE_OK) {
        fprintf(stderr, "parseImage: Failed to set error buffer [%d]: %s\n", code, g_curl_error_buffer);
        curl_easy_cleanup(conn);
        return imageUrl;
    }

    code = curl_easy_setopt(conn, CURLOPT_URL, url.c_str());
    if (code != CURLE_OK) {
        fprintf(stderr, "parseImage: Failed to set URL [%s]\n", g_curl_error_buffer);
        curl_easy_cleanup(conn);
        return imageUrl;
    }

    code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
    if (code != CURLE_OK) {
        fprintf(stderr, "parseImage: Failed to set redirect option [%s]\n", g_curl_error_buffer);
        curl_easy_cleanup(conn);
        return imageUrl;
    }

    code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
    if (code != CURLE_OK) {
        fprintf(stderr, "parseImage: Failed to set writer [%s]\n", g_curl_error_buffer);
        curl_easy_cleanup(conn);
        return imageUrl;
    }

    g_curl_write_buffer.clear();
    code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &g_curl_write_buffer);
    if (code != CURLE_OK) {
        fprintf(stderr, "parseImage: Failed to set write data [%s]\n", g_curl_error_buffer);
        curl_easy_cleanup(conn);
        return imageUrl;
    }

    code = curl_easy_perform(conn);
    if (code != CURLE_OK) {
        fprintf(stderr, "parseImage: Failed to get '%s': %s\n", url.c_str(), g_curl_error_buffer);
    }

    htmlDocPtr doc = NULL;
    xmlXPathContextPtr xpathCtx = NULL;
    xmlXPathObjectPtr xpathObj = NULL;

    if (!g_curl_write_buffer.empty()) {
        doc = htmlReadMemory(g_curl_write_buffer.c_str(),
                             (int)g_curl_write_buffer.length(),
                             NULL,
                             "UTF-8",
                             HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);

        if (doc == NULL) {
            fprintf(stderr, "parseImage: Failed to parse HTML from buffer.\n");
        } else {
            xpathCtx = xmlXPathNewContext(doc);
            if (xpathCtx == NULL) {
                fprintf(stderr, "parseImage: Failed to create XPath context.\n");
            } else {
                const xmlChar* xpathExpr = (const xmlChar*)"//meta[@property='og:image']";
                xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);

                if (xpathObj != NULL) {
                    xmlNodeSetPtr nodes = xpathObj->nodesetval;
                    if (nodes && nodes->nodeNr > 0) {
                        xmlNodePtr cur = nodes->nodeTab[0];
                        xmlChar *contentVal = xmlGetProp(cur, (const xmlChar*)"content");
                        if (contentVal) {
                            imageUrl = (char*)contentVal;
                            xmlFree(contentVal);
                        }
                    }
                }
            }
        }
    }

    if (xpathObj != NULL) xmlXPathFreeObject(xpathObj);
    if (xpathCtx != NULL) xmlXPathFreeContext(xpathCtx);
    if (doc != NULL) xmlFreeDoc(doc);

    curl_easy_cleanup(conn);

    return imageUrl;
}

std::vector<std::vector<std::string>> parseRSS(std::string url) {
    std::vector<std::vector<std::string>> arr;
    CURL *curl_handle = NULL;
    CURLcode res;
    std::string rss_content_buffer;

    curl_handle = curl_easy_init();
    if (curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &rss_content_buffer);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, g_curl_error_buffer);

        res = curl_easy_perform(curl_handle);

        if (res != CURLE_OK) {
            fprintf(stderr, "parseRSS: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl_handle);
    } else {
        fprintf(stderr, "parseRSS: curl_easy_init() failed\n");
    }

    if (rss_content_buffer.empty()) {
        fprintf(stderr, "parseRSS: Failed to retrieve RSS content or content is empty.\n");
        return arr;
    }

    xmlDocPtr doc = NULL;
    xmlNodePtr cur = NULL;

    doc = xmlReadMemory(rss_content_buffer.c_str(), rss_content_buffer.length(), "noname.xml", NULL, 0);

    if (doc == NULL) {
        fprintf(stderr, "parseRSS: Failed to parse XML from buffer.\n");
        return arr;
    }

    cur = xmlDocGetRootElement(doc);

    if (cur == NULL) {
        fprintf(stderr, "parseRSS: Empty document.\n");
        xmlFreeDoc(doc);
        return arr;
    }

    if (xmlStrcmp(cur->name, (const xmlChar *)"rss")) {
        fprintf(stderr, "parseRSS: Root element is not 'rss'. Attempting to parse as RSS anyway.\n");
    }

    cur = cur->children;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"channel"))) {
            xmlNodePtr channel_node = cur->children;
            while (channel_node != NULL) {
                if ((!xmlStrcmp(channel_node->name, (const xmlChar *)"item"))) {
                    std::string title = "";
                    std::string link = "";
                    std::string image = "";

                    xmlNodePtr item_node = channel_node->children;
                    while (item_node != NULL) {
                        if ((!xmlStrcmp(item_node->name, (const xmlChar *)"title"))) {
                            xmlChar *key = xmlNodeGetContent(item_node);
                            if (key) {
                                title = (char*)key;
                                xmlFree(key);
                            }
                        } else if ((!xmlStrcmp(item_node->name, (const xmlChar *)"link"))) {
                            xmlChar *key = xmlNodeGetContent(item_node);
                            if (key) {
                                link = (char*)key;
                                xmlFree(key);
                            }
                        }
                        item_node = item_node->next;
                    }
                    if (!link.empty()) {
                        image = parseImage(link); 
                    }
                    arr.push_back({title, link, image});
                }
                channel_node = channel_node->next;
            }
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc); 

    return arr;
}



#endif 
