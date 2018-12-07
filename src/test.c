#include "rikerio.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define PROFILE "default"

int main() {

    printf("Listing all profiles ... ");

    int retVal = 0;

    unsigned int profileCount = 0;

    retVal  = rio_profile_count(&profileCount);

    if (-1 == retVal) {
        fprintf(stderr, "Error counting profiles.\n");
        return EXIT_FAILURE;
    }

    rio_profile_t* profiles = calloc(1, sizeof(rio_profile_t) * profileCount);

    retVal = rio_profile_get(profiles);

    if (-1 == retVal) {
        printf("failed (%s)!\n", strerror(errno));
        return EXIT_FAILURE;
    } else {

        printf("successfull!\n");

        for (unsigned int i = 0; i < profileCount; i += 1) {

            printf("- %s\n", profiles[i]);

        }

    }


    char* ptr;
    uint32_t offset = 0;
    uint32_t size = 400;

    printf("Allocation %d bytes of memory ... ", size);

    int retValAlloc = rio_alloc_add(PROFILE, size, &ptr, &offset);

    if (retValAlloc == -1) {
        printf("failed allocating memory (%s).\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("successfull with offset %d.\n", offset);

    printf("List links ... ");

    unsigned int linkCount = 0;

    if (rio_link_count(PROFILE, &linkCount) == -1) {
        fprintf(stderr, "Error counting links.\n");
        return EXIT_FAILURE;
    }

    rio_link_t* linkList = calloc(linkCount, sizeof(rio_link_t));

    int retListLinks = rio_link_get(PROFILE, linkList);

    if (retListLinks == -1) {
        printf("failed.\n");
        return EXIT_FAILURE;
    } else {

        printf("successfull.\n");

        for (int i = 0; i < retListLinks; i += 1) {

            printf("  %s\n", linkList[i]);

        }

    }



    unsigned int index = 0;

    while (index < 100) {

        rio_link_t link;

        sprintf(link, "in.data[%d]", index % 10);

        rio_adr_t adr = { index % 12, index % 8 };

        printf("Adding link %s ... ", link);

        if (rio_link_adr_add(PROFILE, link, adr) == -1) {
            printf("failed.\n");
            return -1;
        }

        printf("successfull.\n");

        printf("Getting link %s ... ", link);

        unsigned int linkCount = 0;

        if (rio_link_adr_count(PROFILE, link, &linkCount) == -1) {
            fprintf(stderr, "Error getting adr count.\n");
            return -1;
        }

        rio_adr_t* adrList = calloc(linkCount, sizeof(rio_adr_t));

        if (rio_link_adr_get(PROFILE, link, adrList) == -1) {

            printf("failed\n");
            return -1;

        } else {

            printf("successfull.\n");

            for (unsigned int i = 0; i < linkCount; i += 1) {

                printf("  Adr %2d = %d.%d.\n" , (i+1), adrList[i].byteOffset, adrList[i].bitOffset);

            }

        }

        free(adrList);

        index = (index + 1);

        usleep(20000);

    }

    /* removing all links */

    for (unsigned int i = 0; i < 9; i+= 1) {

        rio_link_t link;
        sprintf(link, "in.data[%d]", i);

        printf("Removing link %s ... ", link);

        if (rio_link_rm(PROFILE, link) == -1) {
            printf("failed!\n");
        } else {
            printf("successfull.\n");
        }

    }

    retVal = rio_alias_link_add(PROFILE, "test-alias-01", "some-real-link");

    if (-1 == retVal) {
        fprintf(stderr, "Error adding alias (%s).\n", strerror(errno));
    }

    retVal = rio_alias_link_add(PROFILE, "test-alias-01", "some-other-real-link");

    if (-1 == retVal) {
        fprintf(stderr, "Error adding alias (%s).\n", strerror(errno));
    }

    retVal = rio_alias_link_rm(PROFILE, "test-alias-01", "some-noexisting-link");

    if (-1 == retVal) {
        fprintf(stderr, "Error removing link (%s).\n", strerror(errno));
    }

    retVal = rio_alias_link_rm(PROFILE, "test-alias-01", "some-real-link");

    if (-1 == retVal) {
        fprintf(stderr, "Error removing link (%s).\n", strerror(errno));
    }

    retVal = rio_alias_link_rm(PROFILE, "test-alias-01", "some-other-real-link");

    if (-1 == retVal) {
        fprintf(stderr, "Error removing link (%s).\n", strerror(errno));
    }


    /* do it again */

    retVal = rio_alias_link_add(PROFILE, "test-alias-01", "some-real-link");

    if (-1 == retVal) {
        fprintf(stderr, "Error adding alias (%s).\n", strerror(errno));
    }

    retVal = rio_alias_link_add(PROFILE, "test-alias-01", "some-other-real-link");

    if (-1 == retVal) {
        fprintf(stderr, "Error adding alias (%s).\n", strerror(errno));
    }

    retVal = rio_alias_link_rm(PROFILE, "test-alias-01", NULL);

    if (-1 == retVal) {
        fprintf(stderr, "Error removing link (%s).\n", strerror(errno));
    }

    printf("Freeing all allocations from this process ... ");

    int retValFree = rio_alloc_rmall(PROFILE);

    if (retValFree == -1) {
        printf("failed freeing memory (%s).\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("successfull.\n");

    return EXIT_SUCCESS;

}
