#include "astar.h"
#include "common.h"


// Global state to avoid passing it through context
// static struct algorithm_state_t* g_state;
static struct coordinate_t dump[1024];
static int dumpi = 0;

// Helper function to check if a coordinate is within bounds
static int is_valid(int x, int y) {
    return y >= 0 && y < ROWS && x >= 0 && x < COLS;
}

// Helper function to calculate heuristic (Manhattan distance)
static float manhattanDistance(struct coordinate_t a, struct coordinate_t b) {
    return fabsf(a.x - b.x) + fabsf(a.y - b.y);
}

// Implementation of required functions for ASPathNodeSource

void nodeNeighbors(ASNeighborList neighbors, struct coordinate_t *node, struct algorithm_state_t *context) {
    int y = node->y;
    int x = node->x;
    printf("\nAdding neighbours of (%d, %d)\n", x, y);
    
    // Check all 4 directions
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    
    for (int i = 0; i < 4; i++) {
        // printf("")
        int x1 = x + dr[i];
        int y1 = y + dc[i];
        printf("Checking %d, %d (%d)\n", x1, y1, context->mat[y1][x1].passable);

        if (context->mat[y1][x1].passable) {
            dump[dumpi] = (struct coordinate_t){
                .y = y1,
                .x = x1
            };
            printf("Found neighbour: (%d, %d)\n", x1, y1);
            ASNeighborListAdd(neighbors, &dump[dumpi], 1.0f); // Assuming uniform cost of 1
            dumpi++;
        }
    }
    printf("\n");
}


int nodeComparator(struct coordinate_t *node1, struct coordinate_t *node2, struct algorithm_state_t *context) {
   
    AUTO dist1 = manhattanDistance(*node1, context->final);
    AUTO dist2 = manhattanDistance(*node1, context->final);

    return (dist1 < dist2) ? -1 : (dist1 > dist2) ? 1 : 0;
}

float pathCostHeuristic(struct coordinate_t *a, struct coordinate_t *b) {
    return manhattanDistance(*a, *b);
}

// Main function to find the path
ASPath find_shortest_path(struct algorithm_state_t* state) {
    // g = state;
    
    ASPathNodeSource source = {
        .nodeSize = sizeof(struct coordinate_t),
        .nodeNeighbors = nodeNeighbors,
        .pathCostHeuristic = pathCostHeuristic,
        .nodeComparator = NULL,
        .earlyExit = NULL
    };
    
    AUTO start = state->initial; // {{.y = state->initial.y, .x = state->initial.x}, 0, 0};
    AUTO goal = state->final; // {{.y = state->final.y, state->final.x}, 0, 0};
    
    return ASPathCreate(&source, state, &start, &goal);
}

// Function to print the path
void print_path(ASPath path) {
    size_t count = ASPathGetCount(path);
    printf("Path found with %zu steps:\n", count);
    
    for (size_t i = 0; i < count; i++) {
        struct coordinate_t *node = ASPathGetNode(path, i);
        printf("(%d, %d) ", node->x, node->y);
    }
    printf("\n");
}



void findOptimalPath(struct sim_t *sim) {
    printf("--- OPTIMIZE ---\n");
    printf("Starting from: %d, %d\nEnding at: %d, %d\n", 
    sim->algorithm_state.initial.x, sim->algorithm_state.initial.y,
    sim->algorithm_state.final.x, sim->algorithm_state.final.y);
    AUTO path = find_shortest_path(&sim->algorithm_state);
    print_path(path);

    sim->optimalPath.length = ASPathGetCount(path);
    sim->optimalPath.found = true;
    for (size_t i = 0; i < sim->optimalPath.length; i++) {
        struct coordinate_t *node = ASPathGetNode(path, i);
        sim->optimalPath.path[i] = *node;
    }
}