#include "rikerio.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define PROFILE "default"

int main(int argc, char** argv) {


    rio_profile_t* profiles = NULL;

    printf("Listing all profiles ... ");

    int retProfiles = rio_plist(&profiles);

    if (retProfiles == -1) {
        printf("failed (%s)!\n", strerror(errno));
        return EXIT_FAILURE;
    } else {

        printf("successfull!\n");

        for (unsigned int i = 0; i < retProfiles; i += 1) {

            printf("  %s\n", profiles[i]);

        }

    }


    void* ptr;
    uint32_t offset = 0;
    uint32_t size = 400;

    printf("Allocation %d bytes of memory ... ", size);

    int retValAlloc = rio_malloc(PROFILE, 0, size, &ptr, &offset);

    if (retValAlloc == -1) {
        printf("failed allocating memory (%s).\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("successfull with offset %d.\n", offset);

    printf("List links ... ");

    rio_key_t* linkList = NULL;

    int retListLinks = rio_llist(PROFILE, &linkList);

    if (retListLinks == -1) {
        printf("failed.\n");
        return EXIT_FAILURE;
    } else {

        printf("successfull.\n");

        for (unsigned int i = 0; i < retListLinks; i += 1) {

            printf("  %s\n", linkList[i]);

        }

    }



    unsigned int index = 0;

    while (index < 100) {

        rio_key_t link;

        sprintf(link, "in.data[%d]", index % 10);

        rio_adr_t adr = { index % 12, index % 8 };

        printf("Adding link %s ... ", link);

        if (rio_aadd(PROFILE, 0, link, &adr) == -1) {
            printf("failed.\n");
            return -1;
        }

        printf("successfull.\n");

        printf("Getting link %s ... ", link);

        rio_adr_t* adrList = NULL;

        int linkCount = rio_aget(PROFILE, 0, link, &adrList);

        if (linkCount == -1) {

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

        rio_key_t link;
        sprintf(link, "in.data[%d]", i);

        printf("Removing link %s ... ", link);

        if (rio_ldel(PROFILE, 0, link) == -1) {
            printf("failed!\n");
        } else {
            printf("successfull.\n");
        }

    }



    printf("Freeing all allocations from this process ... ");

    int retValFree = rio_mfreeall(PROFILE, 0);

    if (retValFree == -1) {
        printf("failed freeing memory (%s).\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("successfull.\n");

    return EXIT_SUCCESS;

}
