#include "common.h"

static char *getCharForNode(struct node_t n)
{
    if (!n.inspected) return "-";
    int totalExits = n.exits.top + n.exits.left + n.exits.right + n.exits.bottom;
    switch (totalExits)
    {
    case 0:
        return "0";
    case 1:
        return "1";
    case 2:
        return "2";
    case 3:
        return "3";
    case 4:
        return "4";
    default:
        return "X";
    }
}

void printVisitedCells(struct algorithm_state_t *State)
{
    AUTO mat = State->mat;
    for (int i = 0; i < 24; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            if (mat[i][j].visited != false)
                printf("%s ", getCharForNode(mat[i][j]));
            else
                printf("  ");
        }
        printf("\n");
    }
}

char *printNoOfExits(struct packed_exits_t exits)
{
    // static char str[5];
    char *str = (char *)malloc(5);
    sprintf(str, "%c%c%c%c", exits.top ? 'T' : ' ', exits.left ? 'L' : ' ', exits.right ? 'R' : ' ', exits.bottom ? 'B' : ' ');
    return str;
}

struct packed_exits_t getUnusedExits(struct packed_exits_t exits, struct packed_exits_t used)
{
    // return all unused exits
    return (struct packed_exits_t){
        .top = exits.top && !used.top,
        .left = exits.left && !used.left,
        .right = exits.right && !used.right,
        .bottom = exits.bottom && !used.bottom,
    };
}

bool check_bit(unsigned char bits)
{
    return bits && !(bits & (bits-1));
}

char *printOrientation(enum direction_t orientation)
{
    switch (orientation)
    {
    case UP:
        return "UP";
    case DOWN:
        return "DOWN";
    case LEFT:
        return "LEFT";
    case RIGHT:
        return "RIGHT";
    default:
        return "INVALID";
    }
}

unsigned short coordinateToUShort(struct coordinate_t coord)
{
    return *(unsigned short*)&coord;
};