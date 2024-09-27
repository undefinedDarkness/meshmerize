#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DEBUG
#define MAZE_SIZE 100
#define ROWS 100
#define COLS 100

typedef unsigned char uchar;

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
    uchar x;
    uchar y;
    enum direction_t direction;
};

struct sensors_t
{
    bool left;
    bool front;
    bool right;
};
struct coordinate_t
{
    uchar x;
    uchar y;
};

struct packed_exits_t
{
    bool top : 1;
    bool left : 1;
    bool right : 1;
    bool bottom : 1;
};

struct node_t
{
    bool visited : 1;
    bool inspected : 1;
    bool passable: 1;
    union
    {
        struct packed_exits_t exits;
        unsigned char exitsAsChar;
    };
    union
    {
        struct packed_exits_t used;
        unsigned char usedAsChar;
    };
};

struct algorithm_state_t
{
    struct node_t mat[ROWS][COLS];
    struct coordinate_t initial;
    struct coordinate_t final;
    bool finished;
};

struct sim_t
{
    int tick;
    bool finished;
    struct position_t player;
    Image maze;
    char *followed_path;
    struct algorithm_state_t algorithm_state;
    struct {
        struct coordinate_t path[1024];
        int length;
        bool found;
    } optimalPath;
};

// -- SIMULATION --
bool straight(struct sim_t *);
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
void initAlgorithm(struct sim_t *, struct algorithm_state_t *);
struct sensors_t getSensorsOneStepAhead(struct sim_t *sim);

#ifdef __cplusplus
#define AUTO auto
#else
#define AUTO __auto_type
#endif

// -- ALGORITHM --
bool movedHook(struct sim_t *sim);
void newSensorDataHook(struct sim_t *sim);
void algorithm(struct sim_t *, struct sensors_t, struct algorithm_state_t *);
void printVisitedCells(struct algorithm_state_t *State);
// void algorithm(void *sim, struct sensors_t S, int x, int y, int visited[24][24]);

// -- PRINTING --
char *printNoOfExits(struct packed_exits_t exits);
bool check_bit(unsigned char bits);
void faceNorth(struct sim_t *sim);
char *printOrientation(enum direction_t orientation);
struct packed_exits_t getUnusedExits(struct packed_exits_t exits, struct packed_exits_t used);

// -- OPTIMIZATION --
void findOptimalPath(struct sim_t *state);
unsigned short coordinateToUShort(struct coordinate_t coord);