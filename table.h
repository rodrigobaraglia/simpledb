#ifndef _TABLE_H_
#define _TABLE_H_
#include <stdint.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define TABLE_MAX_PAGES 100

typedef struct Row
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct Table
{
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;

void serialize_row(Row *source, void *destination);
void deserialize_row(void *source, Row *destination);
void *row_slot(Table *table, uint32_t row_num);
Table *new_table();
void free_table(Table *table);
void print_row(Row *row);

#endif