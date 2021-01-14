#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "constants.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Must supply a database filename\n");
        exit(EXIT_FAILURE);
    }
    char *filename = argv[1];
    Table *table = db_open(filename);
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
        case (PREPARE_STRING_TOO_LONG):
            printf("String is too long.\n");
            continue;
        case (PREPARE_NEGATIVE_ID):
            printf("ID must be positive.\n");
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
