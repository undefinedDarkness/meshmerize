#include "common.h"

// https://forum.pololu.com/t/maze-solving-with-loop/22889

// extern "C"
// {

void initAlgorithm(struct algorithm_state_t *State)
{
    State->initialPosition[0] = 21;
    State->initialPosition[1] = 15;
    for (int i = 0; i < 24; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            State->mat[i][j].visited = false;
            State->mat[i][j].exitsAsChar = 0;
            State->mat[i][j].used.top = false;
            State->mat[i][j].used.left = false;
            State->mat[i][j].used.right = false;
            State->mat[i][j].used.bottom = false;
        }
    }
}

static char *getCharForNode(struct node_t n)
{
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

static char *printNoOfExits(struct packed_exits_t exits)
{
    char *str = (char *)malloc(5);
    sprintf(str, "%c%c%c%c", exits.top ? 'T' : ' ', exits.left ? 'L' : ' ', exits.right ? 'R' : ' ', exits.bottom ? 'B' : ' ');
    return str;
}

static char *printOrientation(enum direction_t orientation)
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
    }
}

/*

    Right now, exit detection isn't perfect but it mostly works?
    Hopefully it'll work enough to use

 */

void newSensorDataHook(struct sim_t *sim)
{
    AUTO S = getSensors(sim);
    AUTO S1 = getSensorsOneStepAhead(sim);
    AUTO Orientation = getOrientation(sim);
    AUTO P = getLookingAtPoint(sim);

    #define Pt sim->algorithm_state.mat[P.y][P.x]

    if (Pt.inspected == true)
        return;

    Pt.inspected = true;

    switch (Orientation) {
        case UP:
            Pt.exits.left = S.left;
            Pt.exits.right = S.right;
            Pt.exits.top = S1.front;
            break;
        case DOWN:
            Pt.exits.bottom = S1.front;
            Pt.exits.right = S.left;
            Pt.exits.left = S.right;
            break;
        case LEFT:
            Pt.exits.left = S1.front;
            Pt.exits.top = S.right;
            Pt.exits.bottom = S.left;
            break;
        case RIGHT:
            Pt.exits.top = S.left;
            Pt.exits.bottom = S.right;
            Pt.exits.right = S1.front;
            break;
    }
}

void movedHook(struct sim_t *sim)
{
    AUTO P = getCurrentPoint(sim);
    sim->algorithm_state.mat[P.y][P.x].visited = true;
}

void algorithm(struct sim_t *sim, struct sensors_t S, struct algorithm_state_t *State)
{

    if (S.front && S.left && S.right)
    {
        straight(sim);

        S = getSensors(sim);

        if (S.front && S.left && S.right)
        {
            done(sim);
        }
        else
        {
            turnLeft(sim);
        }
    }
    else if (S.left)
    {
        straight(sim);

        turnLeft(sim);
    }
    else if (S.front && S.right)
    {
        straight(sim);

        S = getSensors(sim);
        if (!S.front)
        {
            turnRight(sim);
        }
    }
    else if (S.front)
    {
        straight(sim);
    }
    else if (S.right)
    {
        straight(sim);
        turnRight(sim);
    }
    else
    {
        turnAround(sim);
    }
}
