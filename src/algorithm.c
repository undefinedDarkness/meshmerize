#include "common.h"
#define Pt sim->algorithm_state.mat[P.y][P.x]

// https://forum.pololu.com/t/maze-solving-with-loop/22889

// extern "C"
// {

void initAlgorithm(struct sim_t *sim, struct algorithm_state_t *State)
{
    State->initial = getCurrentPoint(sim);
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


/*

    Right now, exit detection isn't perfect but it mostly works?
    Hopefully it'll work enough to use

 */

void usedExit(struct sim_t *sim, enum direction_t ndir) {
    ndir %= 4;
    // AUTO ndir = directionRelativeToOrientation(getOrientation(sim), dir);
    AUTO P = getCurrentPoint(sim);
    printf("Setting used exit for (%d, %d) in direction %s\n", P.x, P.y, printOrientation(ndir));
    switch (ndir) {
        case UP:
            Pt.used.top = true;
            break;
        case DOWN:
            Pt.used.bottom = true;
            break;
        case LEFT:
            Pt.used.left = true;
            break;
        case RIGHT:
            Pt.used.right = true;
            break;
        default:
            printf("GOT INVALID EXIT DIRECTION: %d\n", ndir);
            break;
    }
}

void newSensorDataHook(struct sim_t *sim)
{
    AUTO S = getSensors(sim);
    AUTO S1 = getSensorsOneStepAhead(sim);
    AUTO P1 = getCurrentPoint(sim);
    sim->algorithm_state.mat[P1.y][P1.x].visited = true;
    sim->algorithm_state.mat[P1.y][P1.x].passable = true;
    AUTO Orientation = getOrientation(sim);
    AUTO P = getLookingAtPoint(sim);

    if (S.front)
        Pt.passable = true;

    if (Pt.inspected == true)
        return;

    Pt.inspected = true;

    switch (Orientation)
    {
    case UP:
        Pt.exits.left = S.left;
        Pt.exits.right = S.right;
        Pt.exits.top = S1.front;
        sim->algorithm_state.mat[P.y][P.x - 1].passable = S.left;
        sim->algorithm_state.mat[P.y][P.x + 1].passable = S.right;
        sim->algorithm_state.mat[P.y - 1][P.x].passable = S1.front;
        // Pt.exits.bottom = true;
        break;
    case DOWN:
        Pt.exits.bottom = S1.front;
        Pt.exits.right = S.left;
        Pt.exits.left = S.right;
        sim->algorithm_state.mat[P.y + 1][P.x].passable = S1.front;
        sim->algorithm_state.mat[P.y][P.x - 1].passable = S.right;
        sim->algorithm_state.mat[P.y][P.x + 1].passable = S.left;
        // Pt.exits.top = true;
        break;
    case LEFT:
        Pt.exits.left = S1.front;
        Pt.exits.top = S.right;
        Pt.exits.bottom = S.left;
        sim->algorithm_state.mat[P.y][P.x - 1].passable = S1.front;
        sim->algorithm_state.mat[P.y - 1][P.x].passable = S.right;
        sim->algorithm_state.mat[P.y + 1][P.x].passable = S.left;
        // Pt.exits.right = true;
        break;
    case RIGHT:
        Pt.exits.top = S.left;
        Pt.exits.bottom = S.right;
        Pt.exits.right = S1.front;
        sim->algorithm_state.mat[P.y][P.x + 1].passable = S1.front;
        sim->algorithm_state.mat[P.y - 1][P.x].passable = S.left;
        sim->algorithm_state.mat[P.y + 1][P.x].passable = S.right;
        // Pt.exits.left = true;
        break;
    }

    printf("Set (%d, %d) to %s [%s] (%d) [%d %d %d] [%d %d %d]\n", P.x, P.y, printNoOfExits(Pt.exits), printOrientation(Orientation),
            Pt.exits.top + Pt.exits.bottom + Pt.exits.left + Pt.exits.right,
           S.left, S.front, S.right,
           S1.left, S1.front, S1.right);
}

bool movedHook(struct sim_t *sim)
{
    AUTO P = getCurrentPoint(sim);
    AUTO orientation = getOrientation(sim);

    if (Pt.visited && !check_bit(Pt.exitsAsChar)) {
        AUTO unused = getUnusedExits(Pt.exits, Pt.used);
        printf("Visiting same point (%d, %d)! Point has %s exits, So far have used %s exits, Unused exits are %s\n", 
            P.x, P.y,
        printNoOfExits(Pt.exits), printNoOfExits(Pt.used), printNoOfExits(unused));

        // Use the left most unused exits
        faceNorth(sim);

        if (unused.left && orientation != RIGHT) {
            printf("Using left exit\n");
            turnLeft(sim);
            usedExit(sim, LEFT);
        } else if (unused.top && orientation != DOWN) {
            printf("Using top exit\n");
            straight(sim);
            usedExit(sim, UP);
        } else if (unused.right && orientation != LEFT) {
            printf("Using right exit\n");
            turnRight(sim);
            usedExit(sim, RIGHT);
        } else if (unused.bottom && orientation != UP) {
            printf("Using bottom exit\n");
            turnAround(sim);
            usedExit(sim, DOWN);
        } else {
            return false;
        }

        return true;
    }     
    return false;
}



void algorithm(struct sim_t *sim, struct sensors_t S, struct algorithm_state_t *State)
{
    AUTO P = getLookingAtPoint(sim);
    AUTO O = getOrientation(sim);
    // To turn clock wise, add 1
    // To turn anti clock wise, subtract 1

    if (S.front && S.left && S.right)
    {
        if (straight(sim)) return;
        Pt.visited = true;

        S = getSensors(sim);

        if (S.front && S.left && S.right)
        {
            State->final = getCurrentPoint(sim);
            State->finished = true;
            done(sim);
        }
        else
        {
            turnLeft(sim);
            usedExit(sim, O - 1);
        }
    }
    else if (S.left)
    {
        if (straight(sim)) return;
        Pt.visited = true;
        turnLeft(sim);
        usedExit(sim, O - 1);
    }
    else if (S.front && S.right)
    {
        if (straight(sim)) return;
        Pt.visited = true;

        S = getSensors(sim);
        if (!S.front)
        {
            turnRight(sim);
            usedExit(sim, O + 1);
        } else {
            usedExit(sim, O);
        }
    }
    else if (S.front)
    {
        if (straight(sim)) return;
        Pt.visited = true;
        usedExit(sim, O);
    }
    else if (S.right)
    {
        if (straight(sim)) return;
        Pt.visited = true;
        turnRight(sim);
        usedExit(sim, O + 1);
    }
    else
    {
        turnAround(sim);
        usedExit(sim, O + 2);
    }
}
