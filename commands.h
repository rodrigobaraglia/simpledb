#ifndef _COMMANDS_H_
#define _COMMANDS_H_
#include "table.h"

typedef struct InputBuffer
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;

} InputBuffer;

typedef enum MetaCommandResult
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum PrepareResult
{
    PREPARE_SUCCES,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum ExecuteResult
{
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} ExecuteResult;

typedef enum StatementType
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct Statement
{
    StatementType type;
    Row row_to_insert;
} Statement;

ExecuteResult execute_insert(Statement *statement, Table *table);
ExecuteResult execute_select(Statement *statement, Table *table);
ExecuteResult execute_statement(Statement *statement, Table *table);
MetaCommandResult do_meta_command(InputBuffer *ib, Table *table);
PrepareResult prepare_statement(InputBuffer *ib, Statement *statement);
void print_prompt();
void read_input(InputBuffer *ib);
InputBuffer *new_input_buffer();
void close_input_buffer(InputBuffer *ib);

#endif