import pyray as pr
from enum import Enum

class direction(Enum):
    TOP = 'TOP'
    LEFT = 'LEFT'
    RIGHT = 'RIGHT'
    DOWN = 'DOWN'

# Helper functions
def is_color(x: pr.Color, y: pr.Color):
    return x.r == y[0] and x.g == y[1] and x.b == y[2]  # and x.a == y.a

def is_not_color(x, y):
    return not is_color(x, y)

def enc_coord(s):
    return (s['x'] << 16) | s['y']


# Simulation state class
class SimulationState:
    def __init__(self, x: int, y: int, d: direction):
        self.player = {'x': x, 'y': y, 'direction': d}
        self.maze = None
        self.followed_path = ""
        self.tick = 0
        self.finished = False

    def get_sensors(self):
        img = self.maze
        sensors = {'left': 0, 'front': 0, 'right': 0}
        x = self.player['x']
        y = self.player['y']
        
        if self.player['direction'] == 'UP':
            sensors['left'] = is_not_color(pr.get_image_color(img, x - 1, y - 1), pr.BLACK)
            sensors['front'] = is_not_color(pr.get_image_color(img, x, y - 1), pr.BLACK)
            sensors['right'] = is_not_color(pr.get_image_color(img, x + 1, y - 1), pr.BLACK)
        elif self.player['direction'] == 'RIGHT':
            sensors['left'] = is_not_color(pr.get_image_color(img, x + 1, y - 1), pr.BLACK)
            sensors['front'] = is_not_color(pr.get_image_color(img, x + 1, y), pr.BLACK)
            sensors['right'] = is_not_color(pr.get_image_color(img, x + 1, y + 1), pr.BLACK)
        elif self.player['direction'] == 'DOWN':
            sensors['left'] = is_not_color(pr.get_image_color(img, x + 1, y + 1), pr.BLACK)
            sensors['front'] = is_not_color(pr.get_image_color(img, x, y + 1), pr.BLACK)
            sensors['right'] = is_not_color(pr.get_image_color(img, x - 1, y + 1), pr.BLACK)
        elif self.player['direction'] == 'LEFT':
            sensors['left'] = is_not_color(pr.get_image_color(img, x - 1, y + 1), pr.BLACK)
            sensors['front'] = is_not_color(pr.get_image_color(img, x - 1, y), pr.BLACK)
            sensors['right'] = is_not_color(pr.get_image_color(img, x - 1, y - 1), pr.BLACK)
        
        return sensors

    def invalid_player_state(self):
        return is_color(pr.get_image_color(self.maze, self.player['x'], self.player['y']), pr.BLACK)

    def get_current_point(self):
        return {'x': self.player['x'], 'y': self.player['y']}

    def get_looking_at_point(self):
        dir = self.player['direction']
        pos = self.get_current_point()
        
        if dir == 'UP':
            pos['y'] += 1
        elif dir == 'DOWN':
            pos['y'] -= 1
        elif dir == 'LEFT':
            pos['x'] -= 1
        elif dir == 'RIGHT':
            pos['x'] += 1
        
        return pos

    def next_tick(self, algo):
        if self.finished:
            return
        print("--- TICK ---")
        sensors = self.get_sensors()
        # Placeholder for algorithm function
        algo.run(self, sensors)
        self.tick += 1

    def get_sensors_1fwd(self):
        self.straight(amt=1)
        s = self.get_sensors()
        self.straight(amt=-1)
        return s

    def straight(self, amt=1):
        self.followed_path += 'S'
        print("ACTION: STRAIGHT")
        if self.player['direction'] == 'UP':
            self.player['y'] -= amt
        elif self.player['direction'] == 'RIGHT':
            self.player['x'] += amt
        elif self.player['direction'] == 'DOWN':
            self.player['y'] += amt
        elif self.player['direction'] == 'LEFT':
            self.player['x'] -= amt

    def continuous_straight(self):
        sensors = self.get_sensors()
        while not sensors['left'] and not sensors['right'] and sensors['front']:
            if self.player['direction'] == 'UP':
                self.player['y'] -= 1
            elif self.player['direction'] == 'RIGHT':
                self.player['x'] += 1
            elif self.player['direction'] == 'DOWN':
                self.player['y'] += 1
            elif self.player['direction'] == 'LEFT':
                self.player['x'] -= 1
            sensors = self.get_sensors()

    def turn_around(self):
        print("ACTION: TURN AROUND")
        self.followed_path += 'T'
        if self.player['direction'] == 'UP':
            self.player['direction'] = 'DOWN'
        elif self.player['direction'] == 'DOWN':
            self.player['direction'] = 'UP'
        elif self.player['direction'] == 'LEFT':
            self.player['direction'] = 'RIGHT'
        elif self.player['direction'] == 'RIGHT':
            self.player['direction'] = 'LEFT'

    def get_facing_direction(self):
        return self.player['direction']

    def turn_left(self):
        print("ACTION: TURN LEFT")
        self.followed_path += 'L'
        if self.player['direction'] == 'UP':
            self.player['direction'] = 'LEFT'
        elif self.player['direction'] == 'LEFT':
            self.player['direction'] = 'DOWN'
        elif self.player['direction'] == 'DOWN':
            self.player['direction'] = 'RIGHT'
        elif self.player['direction'] == 'RIGHT':
            self.player['direction'] = 'UP'

    def turn_right(self):
        print("ACTION: TURN RIGHT")
        self.followed_path += 'R'
        if self.player['direction'] == 'UP':
            self.player['direction'] = 'RIGHT'
        elif self.player['direction'] == 'RIGHT':
            self.player['direction'] = 'DOWN'
        elif self.player['direction'] == 'DOWN':
            self.player['direction'] = 'LEFT'
        elif self.player['direction'] == 'LEFT':
            self.player['direction'] = 'UP'

    def done(self):
        print("ACTION: DONE")
        print("--- FINISHED ---")
        self.finished = True
