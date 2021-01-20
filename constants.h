
#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#include <stdint.h>

const uint32_t ID_SIZE;
const uint32_t USERNAME_SIZE;
const uint32_t EMAIL_SIZE;
const uint32_t ID_OFFSET;
const uint32_t USERNAME_OFFSET;
const uint32_t EMAIL_OFFSET;
const uint32_t ROW_SIZE;
const uint32_t PAGE_SIZE;
const uint32_t ROWS_PER_PAGE;
const uint32_t TABLE_MAX_ROWS;

/*Common Node Header Layout*/
const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + NODE_TYPE_SIZE;
const uint32_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

/*Leaf Node Header Layout */
const uint32_t LEAF_NODE_NUM_CELLS_SIZE;
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET;
const uint32_t LEAF_NODE_HEADER_SIZE;

/*Leaf Node Body Layout */
const uint32_t LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_KEY_OFFSET;
const uint32_t LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET;
const uint32_t LEAF_NODE_CELL_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS;
const uint32_t LEAF_NODE_MAX_CELLS;

#endif
