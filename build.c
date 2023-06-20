#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 1024

#define DEFAULT_CONF "default.dat"
#define OUTPUT_CONF "rule.conf"

#define GFW_URL "https://raw.githubusercontent.com/Loyalsoldier/surge-rules/release/gfw.txt"
#define DIRECT_URL "https://raw.githubusercontent.com/Loyalsoldier/surge-rules/release/direct.txt"
#define CNCIDR_URL "https://raw.githubusercontent.com/Loyalsoldier/surge-rules/release/cncidr.txt"

struct MemoryStruct {
    char* memory;
    size_t size;
};

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

char* getData(const char* url) {
    CURL* curl_handle;
    CURLcode res;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
    chunk.size = 0;           /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);

    /* some servers do not like requests that are made without a user-agent
       field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* get it! */
    res = curl_easy_perform(curl_handle);

    /* check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return NULL;
    } else {
        /*
         * Now, our chunk.memory points to a memory block that is chunk.size
         * bytes big and contains the remote file.
         *
         * Do something nice with it!
         */

        // 按行输出获取到的网页内容
        // char* line = strtok(chunk.memory, "\n");
        // while (line != NULL) {
        //     printf("%s\n", line);
        //     line = strtok(NULL, "\n");
        // }
    }

    char* line = strtok(chunk.memory, "\n");

    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);

    // free(chunk.memory);

    /* we are done with libcurl, so clean it up */
    curl_global_cleanup();
    return line;
}

char* generateDateString() {
    time_t currentTime;
    struct tm* localTime;
    char* dateString = malloc(50 * sizeof(char));

    // 获取当前时间
    currentTime = time(NULL);
    localTime = localtime(&currentTime);

    // 格式化日期字符串
    strftime(dateString, 50, "# Generate date: %Y-%m-%d %H:%M:%S\n", localTime);
    return dateString;
}

int main() {
    FILE *defaultFile, *outputFile;
    char line[MAX_LINE_LENGTH];

    defaultFile = fopen(DEFAULT_CONF, "r");
    if (defaultFile == NULL) {
        printf("Error opening %s\n", DEFAULT_CONF);
        return 1;
    }

    outputFile = fopen(OUTPUT_CONF, "w");
    if (outputFile == NULL) {
        printf("Error opening %s\n", OUTPUT_CONF);
        return 1;
    }

    // 生成日期字符串并写入输出文件
    char* dateString = generateDateString();
    fputs(dateString, outputFile);
    free(dateString);

    // 复制默认配置文件中的内容到输出文件，直到遇到 [Rule]
    while (fgets(line, MAX_LINE_LENGTH, defaultFile) != NULL) {
        if (strcmp(line, "[Rule]\n") == 0) {
            fputs(line, outputFile);
            break;
        }
        fputs(line, outputFile);
    }

    fputs("# GFW Proxy\n", outputFile);
    char* gfw_data = getData(GFW_URL);
    while (gfw_data != NULL) {
        // while (fgets(line, MAX_LINE_LENGTH, gfwFile) != NULL) {
        strcpy(line, gfw_data);
        // 去除行尾的换行符
        line[strcspn(line, "\n")] = '\0';

        // 如果行的开头存在 "."，则去除该开头部分
        if (line[0] == '.') {
            memmove(line, line + 1, strlen(line));
        }

        // 在行前添加"DOMAIN-SUFFIX,"
        fputs("DOMAIN-SUFFIX,", outputFile);

        // 写入原始行内容
        fputs(line, outputFile);

        // 在行后添加",Proxy"
        fputs(",Proxy\n", outputFile);

        gfw_data = strtok(NULL, "\n");
    }
    fputs("\n", outputFile);
    free(gfw_data);

    fputs("# Direct DIRECT\n", outputFile);
    char* direct_data = getData(DIRECT_URL);
    while (direct_data != NULL) {
        // while (fgets(line, MAX_LINE_LENGTH, directFile) != NULL) {
        strcpy(line, direct_data);
        // 去除行尾的换行符
        line[strcspn(line, "\n")] = '\0';

        // 如果行的开头存在 "."，则去除该开头部分
        if (line[0] == '.') {
            memmove(line, line + 1, strlen(line));
        }

        // 在行前添加"DOMAIN-SUFFIX,"
        fputs("DOMAIN-SUFFIX,", outputFile);

        // 写入原始行内容
        fputs(line, outputFile);

        // 在行后添加",DIRECT"
        fputs(",DIRECT\n", outputFile);
        direct_data = strtok(NULL, "\n");
    }
    fputs("\n", outputFile);
    free(direct_data);

    fputs("# CNCIDR DIRECT\n", outputFile);
    char* cncidr_data = getData(CNCIDR_URL);
    while (cncidr_data != NULL) {
        // while (fgets(line, MAX_LINE_LENGTH, cncidrFile) != NULL) {
        strcpy(line, cncidr_data);
        // 去除行尾的换行符
        line[strcspn(line, "\n")] = '\0';

        // 写入原始行内容
        fputs(line, outputFile);

        // 在行后添加",DIRECT"
        fputs(",DIRECT\n", outputFile);
        cncidr_data = strtok(NULL, "\n");
    }
    fputs("\n", outputFile);
    free(cncidr_data);

    fputs("FINAL,proxy\n\n", outputFile);

    // 复制默认配置文件中 [Rule] 后的有效内容到输出文件
    int foundRule = 0;

    while (fgets(line, MAX_LINE_LENGTH, defaultFile) != NULL) {
        if (line[0] == '[') {
            foundRule = 1;
        }

        if (foundRule) {
            fputs(line, outputFile);
        }
    }

    // 关闭文件
    fclose(defaultFile);
    fclose(outputFile);

    return 0;
}
