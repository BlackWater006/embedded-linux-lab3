#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_RECORDS 100
#define MSG_SIZE 256

typedef struct {
    int id;
    char *name;
    int *scores;
} Record;

Record *create_record(int id, const char *name, int num_scores)
{
    if (name == NULL || num_scores < 0)
        return NULL;

    Record *r = malloc(sizeof(*r));
    if (r == NULL)
        return NULL;

    r->id = id;
    r->name = NULL;
    r->scores = NULL;

    r->name = malloc(strlen(name) + 1);
    if (r->name == NULL)
        goto error;

    strcpy(r->name, name);

    if (num_scores > 0) {
        r->scores = malloc(sizeof(*r->scores) * (size_t)num_scores);
        if (r->scores == NULL)
            goto error;

        for (int i = 0; i < num_scores; i++)
            r->scores[i] = id * 10 + i;
    }

    return r;

error:
    free(r->scores);
    free(r->name);
    free(r);
    return NULL;
}

void delete_record(Record *r)
{
    if (r == NULL)
        return;

    free(r->scores);
    free(r->name);
    free(r);
}

void process_messages(int count)
{
    for (int i = 0; i < count; i++) {
        char *buf = malloc(MSG_SIZE);

        if (buf == NULL) {
            fprintf(stderr, "Failed to allocate message buffer\n");
            return;
        }

        snprintf(buf, MSG_SIZE,
                 "Message #%d: data payload here", i);

        if (i % 10 == 0)
            printf("  Processed: %s\n", buf);

        free(buf);
    }
}

int main(void)
{
    printf("=== Leaky App - LAB-03 Memory Analysis (Fixed) ===\n");

    printf("[1] Creating %d records...\n", NUM_RECORDS);

    Record **records = calloc(NUM_RECORDS, sizeof(*records));
    if (records == NULL) {
        fprintf(stderr, "Failed to allocate record array\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM_RECORDS; i++) {
        records[i] = create_record(i, "EmbeddedLinuxStudent", 5);

        if (records[i] == NULL) {
            fprintf(stderr, "Failed to create record %d\n", i);

            for (int j = 0; j < i; j++)
                delete_record(records[j]);

            free(records);
            return EXIT_FAILURE;
        }
    }

    printf("[2] Deleting records (fixed)...\n");

    for (int i = 0; i < NUM_RECORDS; i++) {
        delete_record(records[i]);
        records[i] = NULL;
    }

    free(records);
    records = NULL;

    printf("[3] Processing messages (fixed)...\n");
    process_messages(50);

    printf("Program finished - memory cleanup completed!\n");

    printf("PID: %d\n", getpid());
    printf("Process will stay alive for 60 seconds...\n");
    fflush(stdout);

    sleep(60);

    return EXIT_SUCCESS;
}
