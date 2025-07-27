#include "imageparser.h"
#include <chrono>
#include <random>
#include <sstream>
#include <iostream>
#include <algorithm>


size_t writer(char *data, size_t size, size_t nmemb, void *userp) {
    std::string *writerData = static_cast<std::string*>(userp);
    if(writerData == NULL) {
        fprintf(stderr, "writer: userp is NULL\n");
        return 0;
    }
    writerData->append(data, size * nmemb);
    return size * nmemb;
}

std::string generateFilename(const std::string &original_url) {
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 9999);
    int random_suffix = distrib(gen);

    std::string extension = ".jpg";
    size_t dot_pos = original_url.rfind('.');
    size_t slash_pos = original_url.rfind('/');


    if (dot_pos != std::string::npos && (slash_pos == std::string::npos || dot_pos > slash_pos) && (dot_pos + 1 < original_url.length())) {
        std::string potential_ext = original_url.substr(dot_pos);

        if (potential_ext.length() <= 5 && potential_ext.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.") == std::string::npos) {
            extension = potential_ext;
        }
    }

    std::stringstream ss;
    ss << "image_" << timestamp << "_" << random_suffix << extension;
    return ss.str();
}

std::string downloadImage(const std::string &image_url_to_download) {
    std::string local_file_path = "";
    std::string image_data_buffer;
    char curl_error_buffer_image[CURL_ERROR_SIZE];
    CURL *conn_image = NULL;
    CURLcode code_image;

    conn_image = curl_easy_init();
    if (conn_image == NULL) {
        fprintf(stderr, "download_and_save_image: Failed to create CURL connection\n");
        return "";
    }

    code_image = curl_easy_setopt(conn_image, CURLOPT_ERRORBUFFER, curl_error_buffer_image);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set error buffer [%d]: %s\n", code_image, curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }

    code_image = curl_easy_setopt(conn_image, CURLOPT_TIMEOUT, 3L);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set timeout option [%s]\n", curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }

    code_image = curl_easy_setopt(curl_error_buffer_image, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:140.0) Gecko/20100101 Firefox/140.0");
    if (code_image != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set UserAgent [%s]\n", curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }


    code_image = curl_easy_setopt(conn_image, CURLOPT_URL, image_url_to_download.c_str());
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set URL '%s': %s\n", image_url_to_download.c_str(), curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }

    code_image = curl_easy_setopt(conn_image, CURLOPT_FOLLOWLOCATION, 1L);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set redirect option [%s]\n", curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }

    code_image = curl_easy_setopt(conn_image, CURLOPT_WRITEFUNCTION, writer);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set writer [%s]\n", curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }
    image_data_buffer.clear();
    code_image = curl_easy_setopt(conn_image, CURLOPT_WRITEDATA, &image_data_buffer);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to set write data [%s]\n", curl_error_buffer_image);
        curl_easy_cleanup(conn_image);
        return "";
    }

    code_image = curl_easy_perform(conn_image);
    if (code_image != CURLE_OK) {
        fprintf(stderr, "download_and_save_image: Failed to download image from '%s': %s\n", image_url_to_download.c_str(), curl_error_buffer_image);
    }
    curl_easy_cleanup(conn_image);

    if (image_data_buffer.empty()) {
        fprintf(stderr, "download_and_save_image: Image data buffer is empty for '%s'\n", image_url_to_download.c_str());
        return "";
    }

    std::string unique_filename = generateFilename(image_url_to_download);
    local_file_path = "/tmp/pvm/" + unique_filename;

    FILE* fp = fopen(local_file_path.c_str(), "wb");
    if (fp) {
        fwrite(image_data_buffer.data(), 1, image_data_buffer.size(), fp);
        fclose(fp);
        fprintf(stderr, "Image saved to: %s\n", local_file_path.c_str());
        return "file://" + local_file_path;
    } else {
        perror("Error opening file for writing");
        fprintf(stderr, "download_and_save_image: Failed to open file for writing: %s\n", local_file_path.c_str());
        return "";
    }
}

std::string extractImageUrl(std::string url) {
    std::string og_image_url = "";
    CURL *conn = NULL;
    CURLcode code;
    std::string curl_write_buffer;
    char curl_error_buffer[CURL_ERROR_SIZE];

    conn = curl_easy_init();
    if (conn == NULL) {
        fprintf(stderr, "extractOgImageUrl: Failed to create CURL connection\n");
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, curl_error_buffer);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set error buffer [%d]: %s\n", code, curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_URL, url.c_str());
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set URL [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_TIMEOUT, 3L);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set timeout option [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:140.0) Gecko/20100101 Firefox/140.0");
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set UserAgent [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set redirect option [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set writer [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    curl_write_buffer.clear();
    code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &curl_write_buffer);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to set write data [%s]\n", curl_error_buffer);
        curl_easy_cleanup(conn);
        return og_image_url;
    }

    code = curl_easy_perform(conn);
    if (code != CURLE_OK) {
        fprintf(stderr, "extractOgImageUrl: Failed to get '%s': %s\n", url.c_str(), curl_error_buffer);
    }

    htmlDocPtr doc = NULL;
    xmlXPathContextPtr xpathCtx = NULL;
    xmlXPathObjectPtr xpathObj = NULL;

    if (!curl_write_buffer.empty()) {
        doc = htmlReadMemory(curl_write_buffer.c_str(),
                             (int)curl_write_buffer.length(),
                             NULL,
                             "UTF-8",
                             HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);

        if (doc == NULL) {
            fprintf(stderr, "extractOgImageUrl: Failed to parse HTML from buffer.\n");
        } else {
            xpathCtx = xmlXPathNewContext(doc);
            if (xpathCtx == NULL) {
                fprintf(stderr, "extractOgImageUrl: Failed to create XPath context.\n");
            } else {
                const xmlChar* xpathExpr = (const xmlChar*)"//meta[@property='og:image']";
                xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);

                if (xpathObj != NULL) {
                    xmlNodeSetPtr nodes = xpathObj->nodesetval;
                    if (nodes && nodes->nodeNr > 0) {
                        xmlNodePtr cur = nodes->nodeTab[0];
                        xmlChar *contentVal = xmlGetProp(cur, (const xmlChar*)"content");
                        if (contentVal) {
                            og_image_url = (char*)contentVal;
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

    return og_image_url;
}

void *parseRssItemThread(void *arg) {
    ParseItemData* itemData = static_cast<ParseItemData*>(arg);
    std::string og_image_url = "";
    const int MAX_RETRIES = 3;
    if (itemData->imageUrl.empty()){
        for (int i = 0; i < MAX_RETRIES; ++i) {
            og_image_url = extractImageUrl(itemData->link);
            if (!og_image_url.empty()) {
                fprintf(stderr, "Found Open Graph image URL on attempt %d for link: %s\n", i + 1, itemData->link.c_str());
                break;
            }
            fprintf(stderr, "Attempt %d: No Open Graph image URL found for link: %s. Retrying...\n", i + 1, itemData->link.c_str());
            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } else {
        og_image_url = itemData->imageUrl;
    }

    if (!og_image_url.empty()) {
        for (int i = 0; i < MAX_RETRIES; ++i) {
            itemData->imageUrl = downloadImage(og_image_url);
            if (!itemData->imageUrl.empty()) {
                fprintf(stderr, "Downloaded image on attempt %d for link: %s\n", i + 1, itemData->link.c_str());
                break;
            }
            fprintf(stderr, "Attempt %d: No image downloaded for link: %s. Retrying...\n", i + 1, itemData->link.c_str());
        }
    } else {
        itemData->imageUrl = "";
        fprintf(stderr, "No image found after %d attempts for link: %s\n", MAX_RETRIES, itemData->link.c_str());
    }



    return NULL;
}

std::vector<std::vector<std::string> > parseRSS(std::string url) {
    std::vector<std::vector<std::string>> final_arr;
    CURL *curl_handle = NULL;
    CURLcode res;
    std::string rss_content_buffer;
    char curl_error_buffer_main[CURL_ERROR_SIZE];

    curl_handle = curl_easy_init();
    if (curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &rss_content_buffer);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, curl_error_buffer_main);

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
        return final_arr;
    }

    xmlDocPtr doc = NULL;
    xmlNodePtr cur = NULL;

    doc = xmlReadMemory(rss_content_buffer.c_str(), rss_content_buffer.length(), "noname.xml", NULL, 0);

    if (doc == NULL) {
        fprintf(stderr, "parseRSS: Failed to parse XML from buffer.\n");
        return final_arr;
    }

    std::vector<ParseItemData> itemsToProcess;

    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
        fprintf(stderr, "parseRSS: Empty document.\n");
        xmlFreeDoc(doc);
        return final_arr;
    }

    if (!xmlStrcmp(cur->name, (const xmlChar *)"rss")) {
        cur = cur->children;
        while (cur != NULL) {
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"channel"))) {
                xmlNodePtr channel_node = cur->children;
                while (channel_node != NULL) {
                    if ((!xmlStrcmp(channel_node->name, (const xmlChar *)"item"))) {
                        std::string title = "";
                        std::string link = "";
                        std::string rss_image = "";


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
                            } else if ((!xmlStrcmp(item_node->name, (const xmlChar *)"enclosure"))) {
                                xmlChar *url_attr = xmlGetProp(item_node, (const xmlChar *)"url");
                                xmlChar *type_attr = xmlGetProp(item_node, (const xmlChar *)"type");

                                if (type_attr) {
                                    std::string enclosure_type = (char*)type_attr;
                                    std::transform(enclosure_type.begin(), enclosure_type.end(), enclosure_type.begin(), ::tolower);

                                    if (enclosure_type.rfind("image/", 0) == 0) {
                                        if (url_attr) {
                                            rss_image = (char*)url_attr;
                                        }
                                    }
                                    xmlFree(type_attr);
                                }

                                if (url_attr) xmlFree(url_attr);
                            }
                            item_node = item_node->next;
                        }
                        if (!link.empty()) {
                            itemsToProcess.push_back({title, link, rss_image});
                        }
                    }
                    channel_node = channel_node->next;
                }
            }
            cur = cur->next;
        }
    } else {
        fprintf(stderr, "parseRSS: Root element is not 'rss'. Attempting to parse as RSS anyway.\n");
    }
    xmlFreeDoc(doc);

    std::vector<pthread_t> tids;
    tids.reserve(itemsToProcess.size());
    for (size_t i = 0; i < itemsToProcess.size(); ++i) {
        pthread_t tid;
        int error = pthread_create(&tid, NULL, parseRssItemThread, &itemsToProcess[i]);
        if (0 != error) {
            fprintf(stderr, "parseRSS: Couldn't run thread for item %zu (link: %s), errno %d\n", i, itemsToProcess[i].link.c_str(), error);
            // itemsToProcess[i].imageUrl = downloadImage(extractOgImageUrl(itemsToProcess[i].link));
        } else {
            tids.push_back(tid);
        }
    }

    for (size_t i = 0; i < tids.size(); ++i) {
        pthread_join(tids[i], NULL);
    }

    for (const auto& item : itemsToProcess) {
        if (item.imageUrl != ""){
            final_arr.push_back({item.title, item.link, item.imageUrl});
        }
    }

    return final_arr;
}
