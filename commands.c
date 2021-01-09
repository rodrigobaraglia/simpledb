
#include "commands.h"
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#ifdef _WIN32
#include "getline.h"
#endif

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

InputBuffer *new_input_buffer()
{
    InputBuffer *ib = (InputBuffer *)malloc(sizeof(InputBuffer));
    ib->buffer = NULL;
    ib->buffer_length = 0;
    ib->buffer_length = 0;
    return ib;
}

void close_input_buffer(InputBuffer *ib)
{
    free(ib->buffer);
    free(ib);
}