#include "rikerio.h"
#include "iostream"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "cassert"

#define PROFILE "default"

int main() {

    RikerIO::Profile profile;

    int retVal = RikerIO::init("default", profile);

    if (retVal == RikerIO::result_error) {
        printf("failed (%s)!\n", strerror(errno));
        return EXIT_FAILURE;
    }

    assert(strcmp(profile.id, "default") == 0);
    assert(profile.byte_size == 4096);

    RikerIO::Allocation alloc_a;
    RikerIO::Allocation alloc_b;

    RikerIO::alloc(profile, 100, "test", alloc_a);
    RikerIO::alloc(profile, 150, "test-a", alloc_b);

    assert(alloc_a.offset == 0);
    assert(alloc_b.offset == 100);

    RikerIO::dealloc(profile, "test");

    RikerIO::Allocation alloc_c;

    RikerIO::alloc(profile, 75, "test-b", alloc_c);

    assert(alloc_c.offset == 0);

#if 0
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

    retSize = 0;

    int retListLinks = rio_link_get(PROFILE, linkList, linkCount, &retSize);

    if (retListLinks == -1) {
        printf("failed.\n");
        return EXIT_FAILURE;
    } else {

        printf("successfull.\n");

        for (int i = 0; i < retListLinks; i += 1) {

            printf("  %s\n", linkList[i]);

        }

    }


    free(linkList);

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

        if (rio_link_adr_get(PROFILE, link, adrList, linkCount, &retSize) == -1) {

            printf("failed\n");
            return -1;

        } else {

            printf("successfull.\n");

            for (unsigned int i = 0; i < linkCount; i += 1) {

                printf("  Adr %2d = %d.%d.\n", (i + 1), adrList[i].byteOffset, adrList[i].bitOffset);

            }

        }

        free(adrList);

        index = (index + 1);

        usleep(20000);

    }

    /* remove single adr */

    {

        char link[255];
        sprintf(link, "in.data[%d]", 0);

        rio_adr_t adr = { 0, 0 };

        printf("Removing adr %d.%d from link %s ... ", adr.byteOffset, adr.bitOffset, link);

        if (rio_link_adr_rm(PROFILE, link, adr) == -1) {
            printf("failed.\n");
            return -1;
        }

        printf("successfull.\n");

    }


    /* removing all links */

    for (unsigned int i = 0; i < 9; i += 1) {

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

    printf("Reading activity counter again ... ");

    unsigned int newActivityCounter = 0;

    if (rio_profile_counter_get(PROFILE, &newActivityCounter) == -1) {
        printf("failed (%s)\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("success, counter value is at %d.\n", newActivityCounter);

    printf("Comparing first and second activity counter value ... ");
    if (activityCounter == newActivityCounter) {
        printf("failed, %d == %d.\n", activityCounter, newActivityCounter);
        return EXIT_FAILURE;
    }

    printf("success.\n");

    return EXIT_SUCCESS;

#endif

}
