#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG

// NOTE: This must be in a cyclic order
enum direction_t
{
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
};

struct position_t
{
    int x;
    int y;
    enum direction_t direction;
};

struct sensors_t
{
    bool left;
    bool front;
    bool right;
};
struct coordinate_t {
    unsigned short x;
    unsigned short y;
};

struct packed_exits_t {
    bool top:1;
    bool left:1;
    bool right:1;
    bool bottom:1;
};
// what is the value of packed_exits_t if top, left and right are true

struct node_t {
        bool visited:1;
        bool inspected:1;
        // union {
        //     struct packed_exits_t setExits;  
        //     unsigned char setExitsAsChar;
        // };
        union {
            struct packed_exits_t exits;  
            unsigned char exitsAsChar;
         };
        struct packed_exits_t used;
    };

struct algorithm_state_t
{
    struct node_t mat[24][24];
    int initialPosition[2];
};
struct sim_t
{
    int tick;
    bool finished;
    struct position_t player;
    Image maze;
    char *followed_path;
    struct algorithm_state_t algorithm_state;
};





// -- SIMULATION --
void straight(struct sim_t *);
void nextTick(struct sim_t *);
void turnLeft(struct sim_t *);
void turnRight(struct sim_t *);
void turnAround(struct sim_t *);
void continuousStraight(struct sim_t *SimulationState);
struct coordinate_t getLookingAtPoint(struct sim_t *SimulationState);
struct coordinate_t getCurrentPoint(struct sim_t *SimulationState);
void done(struct sim_t *);
struct sensors_t getSensors(struct sim_t *);
bool invalidPlayerState(struct sim_t *SimulationState);
enum direction_t getOrientation(struct sim_t *SimulationState);
void initAlgorithm(struct algorithm_state_t *);
struct sensors_t getSensorsOneStepAhead(struct sim_t *sim);

#define AUTO __auto_type

// -- ALGORITHM --
void movedHook(struct sim_t *sim);
void newSensorDataHook(struct sim_t *sim);
void algorithm(struct sim_t *, struct sensors_t, struct algorithm_state_t *);
void printVisitedCells(struct algorithm_state_t *State);
// void algorithm(void *sim, struct sensors_t S, int x, int y, int visited[24][24]);