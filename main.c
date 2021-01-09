#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32

ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
    static char line[256];
    char *ptr;
    unsigned int len;

    if (lineptr == NULL || n == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if (ferror(stream))
    {
        return -1;
    }

    if (feof(stream))
    {
        return -1;
    }

    fgets(line, 256, stream);

    ptr = strchr(line, '\n');

    if (ptr)
    {
        *ptr = '\0';
    }

    len = strlen(line);

    if ((len + 1) < 256)
    {
        ptr = realloc(*lineptr, 256);
        if (ptr == NULL)
        {
            return -1;
        }
        *lineptr = ptr;
        *n = 256;
    }
    strcpy(*lineptr, line);
    return len;
}

#endif

typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum
{
    PREPARE_SUCCES,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum
{
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} ExecuteResult;

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct
{
    StatementType type;
    Row row_to_insert;
} Statement;

typedef struct
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;

} InputBuffer;

InputBuffer *new_input_buffer()
{
    InputBuffer *ib = (InputBuffer *)malloc(sizeof(InputBuffer));
    ib->buffer = NULL;
    ib->buffer_length = 0;
    ib->buffer_length = 0;
    return ib;
}

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

void serialize_row(Row *source, void *destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void *source, Row *destination)
{
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

const uint32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

typedef struct
{
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;

//Determine where to write/read a particular row:
//(Pointer arithmetic)
void *row_slot(Table *table, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = table->pages[page_num];
    if (page == NULL)
    {
        //Allocate
        page = table->pages[page_num] = malloc(PAGE_SIZE);
        int a = 1;
    }

    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;

    return page + byte_offset;
}

void print_row(Row *row)
{
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

void print_prompt()
{
    printf("db > ");
}

void read_input(InputBuffer *ib)
{
    ssize_t bytes_read = getline(&(ib->buffer), &(ib->buffer_length), stdin);

    ib->input_length = bytes_read;
    ib->buffer[bytes_read] = 0;
}

void close_input_buffer(InputBuffer *ib)
{
    free(ib->buffer);
    free(ib);
}

PrepareResult prepare_statement(InputBuffer *ib, Statement *statement)
{
    if (strncasecmp(ib->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;

        ////***To add later***
        int args_assigned = sscanf(
            ib->buffer,
            "insert %d %s %s",
            &(statement->row_to_insert.id),
            statement->row_to_insert.username,
            statement->row_to_insert.email);

        if (args_assigned < 3)
        {
            return PREPARE_SYNTAX_ERROR;
        }

        return PREPARE_SUCCES;
    }
    if (strcasecmp(ib->buffer, "select") == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCES;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_insert(Statement *statement, Table *table)
{
    if (table->num_rows >= TABLE_MAX_ROWS)
    {
        return EXECUTE_TABLE_FULL;
    }
    Row *row_to_insert = &(statement->row_to_insert);
    serialize_row(row_to_insert, row_slot(table, table->num_rows));
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement *statement, Table *table)
{
    Row row;
    for (uint32_t i = 0; i < table->num_rows; ++i)
    {
        deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement *statement, Table *table)
{
    switch (statement->type)
    {
    case (STATEMENT_INSERT):
        return execute_insert(statement, table);
    case (STATEMENT_SELECT):
        return execute_select(statement, table);
    }
}

Table *new_table()
{
    Table *table = malloc(sizeof(Table));
    table->num_rows = 0;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i)
    {
        table->pages[i] = NULL;
    }
    return table;
}

void free_table(Table *table)
{
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i)
    {
        free(table->pages[i]);
    }
    free(table);
}

MetaCommandResult do_meta_command(InputBuffer *ib, Table *table)
{
    if (strcmp(ib->buffer, ".exit") == 0)
    {
        close_input_buffer(ib);
        free_table(table);
        exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

int main(int argc, char *argv[])
{
    Table *table = new_table();
    InputBuffer *ib = new_input_buffer();

    while (true)
    {
        print_prompt();
        read_input(ib);

        if (ib->buffer[0] == '.')
        {
            switch (do_meta_command(ib, table))
            {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognized command '%s'\n", ib->buffer);
                continue;
            }
        }

        Statement statement;
        switch (prepare_statement(ib, &statement))
        {
        case (PREPARE_SUCCES):
            break;
        case (PREPARE_SYNTAX_ERROR):
            printf("Could not parse statement\n");
            continue;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            if (ib->input_length > 0)
            {
                printf("Unrecognized keyword at start of '%s'\n", ib->buffer);
            }
            continue;
        }

        switch (execute_statement(&statement, table))
        {
        case (EXECUTE_SUCCESS):
            printf("Executed.\n");
            break;
        case (EXECUTE_TABLE_FULL):
            printf("Error: Full Table\n");
            break;
        }
    }
}
