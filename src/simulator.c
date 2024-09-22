#include "common.h"

bool isColor(Color x, Color y)
{
    return x.r == y.r && x.g == y.g && x.b == y.b && x.a == y.a;
}

bool isNotColor(Color x, Color y)
{
    return !(isColor(x, y));
}


struct sensors_t getSensors(struct sim_t *sim_)
{
    struct sim_t sim = *sim_;
    Image img = sim.maze;
    struct sensors_t sensors = {0};
    int x = sim.player.x;
    int y = sim.player.y;
    // printf("GET SENSORS: %d %d\n", x, y);
    // #define GetImageColor(image, x, y) GetImageColor(image, x, y)
    switch (sim.player.direction)
    {
    case UP:
        sensors.left = isNotColor(GetImageColor(img, x - 1, y - 1), BLACK);
        sensors.front = isNotColor(GetImageColor(img, x, y - 1), BLACK);
        sensors.right = isNotColor(GetImageColor(img, x + 1, y - 1), BLACK);
        break;
    case RIGHT:
        sensors.left = isNotColor(GetImageColor(img, x + 1, y - 1), BLACK);
        sensors.front = isNotColor(GetImageColor(img, x + 1, y), BLACK);
        sensors.right = isNotColor(GetImageColor(img, x + 1, y + 1), BLACK);
        break;
    case DOWN:
        sensors.left = isNotColor(GetImageColor(img, x + 1, y + 1), BLACK);
        sensors.front = isNotColor(GetImageColor(img, x, y + 1), BLACK);
        sensors.right = isNotColor(GetImageColor(img, x - 1, y + 1), BLACK);
        break;
    case LEFT:
        sensors.left = isNotColor(GetImageColor(img, x - 1, y + 1), BLACK);
        sensors.front = isNotColor(GetImageColor(img, x - 1, y), BLACK);
        sensors.right = isNotColor(GetImageColor(img, x - 1, y - 1), BLACK);
        break;
    }
    // #undef GetImageColor
    // printf("SENSORS: %d %d %d\n", sensors.left, sensors.front, sensors.right);
    return sensors;
}

bool invalidPlayerState(struct sim_t *SimulationState)
{
    // printf("Color: %d\n", GetImageColor(SimulationState->maze, SimulationState->player.x, SimulationState->player.y).r);
    return isColor(GetImageColor(SimulationState->maze, SimulationState->player.x, SimulationState->player.y), BLACK);
}

struct coordinate_t getCurrentPoint(struct sim_t *SimulationState)
{
    return (struct coordinate_t){
        .x = SimulationState->player.x,
        .y = SimulationState->player.y};
}

enum direction_t getOrientation(struct sim_t *SimulationState)
{
    return SimulationState->player.direction;
}

struct coordinate_t getLookingAtPoint(struct sim_t *SimulationState)
{
    enum direction_t dir = SimulationState->player.direction;
    struct coordinate_t pos = getCurrentPoint(SimulationState);

    if (dir == UP)
    {
        pos.y++;
        return pos;
    }
    else if (dir == DOWN)
    {
        pos.y--;
        return pos;
    }
    else if (dir == LEFT)
    {
        pos.x--;
        return pos;
    }
    else if (dir == RIGHT)
    {
        pos.x++;
        return pos;
    }
}

void nextTick(struct sim_t *SimulationState)
{
    // static int arr[24][24];
    if (SimulationState->finished)
    {
        return;
    }
    printf("--- TICK ---\n");
    // struct position_t prev_player = SimulationState->player;
    struct sensors_t sensors = getSensors(SimulationState);

    // printf("SENSORS: %d %d %d\n", sensors.left, sensors.front, sensors.right);

    algorithm(SimulationState, sensors, &SimulationState->algorithm_state);
    newSensorDataHook(SimulationState);
    // algorithm(SimulationState, sensors, SimulationState->player.x, SimulationState->player.y, arr);

    SimulationState->tick++;
}



void straightAmount(struct sim_t *SimulationState, int amt)
{
    SimulationState->followed_path[SimulationState->tick] = 'S';
    printf("ACTION: STRAIGHT\n");
    switch (SimulationState->player.direction)
    {
    case UP:
        SimulationState->player.y -= amt;
        break;
    case RIGHT:
        SimulationState->player.x += amt;
        break;
    case DOWN:
        SimulationState->player.y += amt;
        break;
    case LEFT:
        SimulationState->player.x -= amt;
        break;
    default:
        printf("INVALID DIRECTION: %d\n", SimulationState->player.direction);
    }
}

void straight(struct sim_t *SimulationState) {
    straightAmount(SimulationState, 1);
    movedHook(SimulationState);
    newSensorDataHook(SimulationState); 
}

struct sensors_t getSensorsOneStepAhead(struct sim_t *sim)
{
    AUTO self = getCurrentPoint(sim);
    switch (getOrientation(sim)) {
        case UP:
            return (struct sensors_t){
                .front = isNotColor(GetImageColor(sim->maze, self.x, self.y - 2), BLACK),
                .left = isNotColor(GetImageColor(sim->maze, self.x - 1, self.y - 2), BLACK),
                .right = isNotColor(GetImageColor(sim->maze, self.x + 1, self.y - 2), BLACK)
            };
        case DOWN:
            return (struct sensors_t){
                .front = isNotColor(GetImageColor(sim->maze, self.x, self.y + 2), BLACK),
                .left = isNotColor(GetImageColor(sim->maze, self.x - 1, self.y + 2), BLACK),
                .right = isNotColor(GetImageColor(sim->maze, self.x + 1, self.y + 2), BLACK)
            };
        case LEFT:
            return (struct sensors_t){
                .left = isNotColor(GetImageColor(sim->maze, self.x - 2, self.y - 1), BLACK),
                .front = isNotColor(GetImageColor(sim->maze, self.x - 2, self.y), BLACK),
                .right = isNotColor(GetImageColor(sim->maze, self.x - 2, self.y + 1), BLACK)
            };
        case RIGHT:
            return (struct sensors_t){
                .left = isNotColor(GetImageColor(sim->maze, self.x + 2, self.y - 1), BLACK),
                .front = isNotColor(GetImageColor(sim->maze, self.x + 2, self.y), BLACK),
                .right = isNotColor(GetImageColor(sim->maze, self.x + 2, self.y + 1), BLACK)
            };
    }
}

void continuousStraight(struct sim_t *SimulationState)
{
    AUTO S = getSensors(SimulationState);
    while (!S.left && !S.right && S.front)
    {
        switch (SimulationState->player.direction)
        {
        case UP:
            SimulationState->player.y--;
            break;
        case RIGHT:
            SimulationState->player.x++;
            break;
        case DOWN:
            SimulationState->player.y++;
            break;
        case LEFT:
            SimulationState->player.x--;
            break;
        default:
            printf("INVALID DIRECTION: %d\n", SimulationState->player.direction);
        }

        S = getSensors(SimulationState);
    }
}

void turnAround(struct sim_t *SimulationState)
{
    printf("ACTION: TURN AROUND\n");
    SimulationState->followed_path[SimulationState->tick] = 'T';
    SimulationState->player.direction = (enum direction_t)((SimulationState->player.direction + 2) % 4);
    newSensorDataHook(SimulationState);
}

enum direction_t getFacingDirection(struct sim_t *SimulationState)
{
    return SimulationState->player.direction;
}

void turnLeft(struct sim_t *SimulationState)
{
    printf("ACTION: TURN LEFT\n");
    SimulationState->followed_path[SimulationState->tick] = 'L';
    SimulationState->player.direction = (enum direction_t)((SimulationState->player.direction + 3) % 4);
    newSensorDataHook(SimulationState);
}

void turnRight(struct sim_t *SimulationState)
{
    printf("ACTION: TURN RIGHT\n");
    SimulationState->followed_path[SimulationState->tick] = 'R';
    SimulationState->player.direction = (enum direction_t)((SimulationState->player.direction + 1) % 4);
    newSensorDataHook(SimulationState);
}

void done(struct sim_t *SimulationState)
{
    printf("ACTION: DONE\n");
    printf("--- FINISHED ---\n");
    SimulationState->finished = true;
}