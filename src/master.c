#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <dlfcn.h>
#include <string.h>
#include <regex.h>
#include <signal.h>
#include <unistd.h>
#include "rikerio.h"
#include "version.h"

static struct option long_options[] = {
    { "conf", required_argument, NULL, 'c' },
    { "prefix", required_argument, NULL, 'p' },
    { "rikerio", required_argument, NULL, 'r' },
    { "id", required_argument, NULL, 'i' },
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { NULL, 0, NULL, 0 }
};

struct byte_conf_st {
    unsigned int size;
    unsigned int count;
};

struct runtime_st {

    int running;

    rio_profile_t profile;
    char prefix[255];

    unsigned int confSize;
    struct byte_conf_st* conf;

} runtime = { 0 };

static void printHelp() {

    printf("Usage: rio-master OPTIONS\n\n");
    printf("Options:\n");
    printf("\t-c|--conf\t\tList of byte sized multiples for example: 8:1;4:2;2:4;1:8,\n\t\t\t\tmeaning eight times one byte data, four times two byte data etc.\n");
    printf("\t-p|--prefix\t\tPrefix for the link descriptions.\n");
    printf("\t-r|--rio\t\tRikerIO Profile.\n");
    printf("\t-v|--version\tPrint version.\n");
    printf("\t-h|--help\t\tPrint this help.\n\n");
    printf("Created by Stefan Pöter<rikerio@cloud-automation.de>.\n");

}

static void printVersion() {
    printf("%s\n", VERSION_SHORT);
}

static void systemd_notify(char* msg) {

    typedef int (*systemd_notify)(int code, const char *param);

    void *systemd_so = dlopen("libsystemd.so", RTLD_NOW);

    if (systemd_so == NULL) {
        printf("No libsystemd.so found. No SystemD notification happening.\n");
        return;
    }

    systemd_notify *sd_notify = (systemd_notify*) dlsym(systemd_so, "sd_notify");
    printf("Notifying systemd (%s).\n", msg);
    systemd_notify func = (systemd_notify) sd_notify;
    int retVal = func(0, msg);

    if (retVal < 0) {
        printf("Error notifying (sd_notify) SystemD (%d).\n", retVal);
    }

    dlclose(systemd_so);

}

static int parseArguments_addConf(struct runtime_st* runtime, char* conf) {

    int pos = strcspn(conf, ":");

    char cntStr[10] = { 0 };
    char sizeStr[10] = { 0 };

    strncpy(cntStr, conf, pos);
    strcpy(sizeStr, conf + pos + 1);

    int cntValue = atoi(cntStr);
    int sizeValue = atoi(sizeStr);

    runtime->confSize += 1;
    runtime->conf = realloc(runtime->conf, runtime->confSize * sizeof(struct byte_conf_st));

    runtime->conf[runtime->confSize - 1] = (struct byte_conf_st) {
        sizeValue, cntValue
    };

}

static int parseArguments_matchConf(char* conf, struct runtime_st* runtime) {

    char tmpString[255] = { 0 };

    regex_t regex;
    int returnValue = 0;
    int retVal = regcomp(&regex, "[1-9][0-9]*\\:[1-9][0-9]*\\;", 0);

    if (retVal) {
        fprintf(stderr, "Internal Error\n");
        return -1;
    }

    unsigned int idx = 0;
    while (!retVal) {
        size_t nmatch = 1;
        regmatch_t matches[1] = { 0 };
        strcpy(tmpString, conf + idx);
        retVal = regexec(&regex, tmpString, nmatch, matches, 0);
        if (!retVal) {

            char tmp[100] = { 0 };
            strncpy(tmp, tmpString, matches[0].rm_eo - matches[0].rm_so - 1);
            parseArguments_addConf(runtime, tmp);

            idx += matches[0].rm_eo;
        } else if (retVal == REG_NOMATCH && strlen(tmpString) == 0) {
            break;
        } else {
            char msgbuf[100];
            regerror(retVal, &regex, msgbuf, sizeof(msgbuf));
            fprintf(stderr, "Error parsing Configuration String %s : %s\n", tmpString, msgbuf);
            regfree(&regex);
            returnValue = -1;
            break;
        }
    }

    regfree(&regex);
    return returnValue;

}

static int parseArguments(int argc, char* argv[], struct runtime_st* runtime) {

    sprintf(runtime->profile, "default");
    sprintf(runtime->prefix, "default");

    runtime->confSize = 0;
    runtime->conf = malloc(1 * sizeof(struct byte_conf_st));

    unsigned int validC = 0;

    while (1) {

        int option_index = 0;
        int c = getopt_long(argc, argv, "c::p::r::hv", long_options, &option_index);

        if (c == -1) {
            break;
        }

        unsigned int v;
        char* s;

        switch (c) {
        case 'c':
            if (parseArguments_matchConf(optarg, runtime) == -1) {
                break;
            }
            validC = 1;
            break;
        case 'p':
            strcpy(runtime->prefix, optarg);
            break;
        case 'r':
            strcpy(runtime->profile, optarg);
            break;
        case 'h':
            printHelp();
            return 0;
        case 'v':
            printVersion();
            return 0;
        case '?':
            printHelp();
            return 0;
        }

    }


    if (!validC || runtime->confSize == 0) {

        printHelp();
        exit(EXIT_FAILURE);

    }

    return 1;

}

void tearDown() {

    runtime.running = 0;

}



int main(int argc, char** argv) {

    int exitValue = 0;

    parseArguments(argc, argv, &runtime);

    printf("Starting RikerIO Master Version %s.\n", VERSION_SHORT);
    printf("Using Prefix : %s.\n", runtime.prefix);
    printf("Using RikerIO Profile : %s.\n", runtime.profile);

    unsigned int overallByteCount = 0;
    for (unsigned int i = 0; i < runtime.confSize; i += 1) {

        printf("Byte Size %d Times %d.\n", runtime.conf[i].size, runtime.conf[i].count);

        overallByteCount += runtime.conf[i].size * runtime.conf[i].count;
    }

    printf("Overall Memory : %d Bytes.\n", overallByteCount);


    uint32_t memoryOffset = 0;
    char* ptr = NULL;

    int retVal = rio_alloc_add(runtime.profile, overallByteCount, &ptr, &memoryOffset);

    if (retVal == -1) {
        fprintf(stderr, "Error allocating memory.\n");
        goto release;
        exitValue = -1;
    }

    printf("Allocated memory on RikerIO.\n");

    uint32_t curOffset = memoryOffset;
    for (unsigned int i = 0; i < runtime.confSize; i += 1) {

        for (unsigned int j = 0; j < runtime.conf[i].count; j += 1) {

            rio_link_t link = { 0 };
            rio_adr_t adr = { curOffset, 0 };

            sprintf(link, "%s.byte_size_%d.channel[%d]", runtime.prefix, runtime.conf[i].size, j + 1);

            curOffset += runtime.conf[i].size;

            printf("Creating link %s at %d.%d.\n", link, adr.byteOffset, adr.bitOffset);

            if (rio_link_adr_add(runtime.profile, link, adr) != 0) {
                fprintf(stderr, "Error creating link %s.\n", link);
            }

        }

    }

    signal(SIGTERM, tearDown);
    signal(SIGINT, tearDown);

    systemd_notify("READY=1");

    runtime.running = 1;

    while (runtime.running) sleep(1);

    systemd_notify("STOPPING=1");


release:

    curOffset = memoryOffset;
    for (unsigned int i = 0; i < runtime.confSize; i += 1) {

        for (unsigned int j = 0; j < runtime.conf[i].count; j += 1) {

            rio_link_t link = { 0 };
            rio_adr_t adr = { curOffset, 0 };

            sprintf(link, "%s.byte_size_%d.channel[%d]", runtime.prefix, runtime.conf[i].size, j + 1);

            curOffset += runtime.conf[i].size;

            printf("Removing link %s at %d.%d.\n", link, adr.byteOffset, adr.bitOffset);

            if (rio_link_adr_rm(runtime.profile, link, adr) != 0) {
                fprintf(stderr, "Error removing link %s.\n", link);
            }

        }

    }

    rio_alloc_rm(runtime.profile, memoryOffset);

    free(runtime.conf);

    return exitValue;

}
