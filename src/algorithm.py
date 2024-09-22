from simulator import SimulationState


class Algorithm:
    def __init__(self):
        self.visited = set()
        self.total_points = 0

    def init_algorithm(self):
        self.visited.clear()
        self.total_points = 0

    def print_visited_cells(self):
        visited_matrix = [[False for _ in range(24)] for _ in range(24)]
        print(f"Visited {self.total_points} points")
        for point in self.visited:
            visited_matrix[point[1]][point[0]] = True
        for row in visited_matrix:
            for cell in row:
                print("1 " if cell else "  ", end="")
            print()

    def run(self, sim: SimulationState, sensors):
        if sensors['front'] and sensors['left'] and sensors['right']:
            sim.straight()
            sensors = sim.get_sensors()
            if sensors['front'] and sensors['left'] and sensors['right']:
                sim.done()
            else:
                sim.turn_left()
        elif sensors['left']:
            sim.straight()
            sim.turn_left()
        elif sensors['front'] and sensors['right']:
            sim.straight()
            sensors = sim.get_sensors()
            if not sensors['front']:
                sim.turn_right()
        elif sensors['front']:
            sim.continuous_straight()
        elif sensors['right']:
            sim.straight()
            sim.turn_right()
        else:
            sim.turn_around()
