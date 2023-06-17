#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 1024

#define DEFAULT_CONF "default.dat"
#define OUTPUT_CONF "rule.conf"
#define GFW_CONF "gfw.txt"
#define DIRECT_CONF "direct.txt"
#define CNCIDR_CONF "cncidr.txt"

#define GFW_URL "https://raw.githubusercontent.com/Loyalsoldier/surge-rules/release/gfw.txt"
#define DIRECT_URL "https://raw.githubusercontent.com/Loyalsoldier/surge-rules/release/direct.txt"
#define CNCIDR_URL "https://raw.githubusercontent.com/Loyalsoldier/surge-rules/release/cncidr.txt"

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

// 回调函数，用于将下载的数据写入文件
static size_t write_callback(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int downloadFile(const char* url, const char* outputFile) {
    CURL* curl;
    FILE* file;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        file = fopen(outputFile, "wb");
        if (file) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                printf("Error downloading file: %s\n", curl_easy_strerror(res));
                fclose(file);
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                return 1;
            }

            fclose(file);
        } else {
            printf("Error opening file for writing: %s\n", outputFile);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return 1;
        }

        curl_easy_cleanup(curl);
    } else {
        printf("Error initializing curl.\n");
        curl_global_cleanup();
        return 1;
    }

    curl_global_cleanup();
    return 0;
}

void removeFile() {
    remove(GFW_CONF);
    remove(DIRECT_CONF);
    remove(CNCIDR_CONF);
}

int main() {
    int gfwResult = downloadFile(GFW_URL, GFW_CONF);
    int directResult = downloadFile(DIRECT_URL, DIRECT_CONF);
    int cncidrResult = downloadFile(CNCIDR_URL, CNCIDR_CONF);

    if (gfwResult || directResult || cncidrResult) {
        printf("Error downloading file.\n");
        removeFile();
        return 1;
    }

    FILE *defaultFile, *outputFile, *gfwFile, *directFile, *cncidrFile;
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
            break;
        }
        fputs(line, outputFile);
    }

    // 添加 [Rule] 标记
    fputs("[Rule]\n", outputFile);

    // 从 GFW_CONF 文件中读取内容并生成规则
    gfwFile = fopen(GFW_CONF, "r");
    if (gfwFile == NULL) {
        printf("Error opening %s\n", GFW_CONF);
        return 1;
    }

    fputs("# GFW Proxy\n", outputFile);
    while (fgets(line, MAX_LINE_LENGTH, gfwFile) != NULL) {
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
    }
    fputs("\n", outputFile);
    fclose(gfwFile);

    // 从 DIRECT_CONF 文件中读取内容并生成规则
    directFile = fopen(DIRECT_CONF, "r");
    if (directFile == NULL) {
        printf("Error opening %s\n", DIRECT_CONF);
        return 1;
    }
    fputs("# Direct DIRECT\n", outputFile);
    while (fgets(line, MAX_LINE_LENGTH, directFile) != NULL) {
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
    }
    fputs("\n", outputFile);
    fclose(directFile);

    // 从 CNCIDR_CONF 文件中读取内容并生成规则
    cncidrFile = fopen(CNCIDR_CONF, "r");
    if (cncidrFile == NULL) {
        printf("Error opening %s\n", CNCIDR_CONF);
        return 1;
    }
    fputs("# CNCIDR DIRECT\n", outputFile);
    while (fgets(line, MAX_LINE_LENGTH, cncidrFile) != NULL) {
        // 去除行尾的换行符
        line[strcspn(line, "\n")] = '\0';

        // 写入原始行内容
        fputs(line, outputFile);

        // 在行后添加",DIRECT"
        fputs(",DIRECT\n", outputFile);
    }
    fputs("\n", outputFile);
    fclose(cncidrFile);

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

    removeFile();

    return 0;
}
