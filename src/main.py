import pyray as pr
from simulator import SimulationState # , next_tick, invalid_player_state
from algorithm import Algorithm

# Constants
MAZE_SCALE = 32
SCALE = lambda x: int(x * MAZE_SCALE)
SCALEX = lambda x: int(x * MAZE_SCALE + MAZE_SCALE / 2)

# Simulation stat

simulation_state = SimulationState(21, 15, 'LEFT')
algorithm = Algorithm()

# Initialize window and textures
pr.set_config_flags(pr.FLAG_VSYNC_HINT | pr.FLAG_WINDOW_HIGHDPI)
pr.init_window(24 * MAZE_SCALE, 24 * MAZE_SCALE, "Hello Raylib")
simulation_state.maze = pr.load_image("maze2.png")
maze_texture = pr.load_texture_from_image(simulation_state.maze)

# Main game loop
while not pr.window_should_close():
    # Drawing
    pr.begin_drawing()
    pr.clear_background(pr.RAYWHITE)
    pr.draw_texture_ex(maze_texture, pr.Vector2(0, 0), 0, MAZE_SCALE, pr.WHITE)
    
    # Draw text
    pr.draw_text(f"Tick: {simulation_state.tick}", 10, 10, 20, pr.RAYWHITE)
    sensors = {'left': 0, 'front': 0, 'right': 0}  # Placeholder for getSensors function
    pr.draw_text(f"Sensors: {sensors['left']} {sensors['front']} {sensors['right']}", 10, 40, 20, pr.RAYWHITE)
    
    # Draw player
    pr.draw_circle(SCALEX(simulation_state.player['x']), SCALEX(simulation_state.player['y']), 8, pr.VIOLET)
    
    # Draw direction indicator
    if simulation_state.player['direction'] == 'LEFT':
        pr.draw_line_ex(
            pr.Vector2(SCALEX(simulation_state.player['x']) - MAZE_SCALE, SCALEX(simulation_state.player['y']) - MAZE_SCALE * 1.5),
            pr.Vector2(SCALEX(simulation_state.player['x']) - MAZE_SCALE, SCALEX(simulation_state.player['y']) + MAZE_SCALE * 1.5),
            MAZE_SCALE, pr.Color(0, 0, 255, 170)
        )
    elif simulation_state.player['direction'] == 'RIGHT':
        pr.draw_line_ex(
            pr.Vector2(SCALEX(simulation_state.player['x']) + MAZE_SCALE, SCALEX(simulation_state.player['y']) - MAZE_SCALE * 1.5),
            pr.Vector2(SCALEX(simulation_state.player['x']) + MAZE_SCALE, SCALEX(simulation_state.player['y']) + MAZE_SCALE * 1.5),
            MAZE_SCALE, pr.Color(0, 0, 255, 170)
        )
    elif simulation_state.player['direction'] == 'UP':
        pr.draw_line_ex(
            pr.Vector2(SCALEX(simulation_state.player['x']) - MAZE_SCALE * 1.5, SCALEX(simulation_state.player['y']) - MAZE_SCALE),
            pr.Vector2(SCALEX(simulation_state.player['x']) + MAZE_SCALE * 1.5, SCALEX(simulation_state.player['y']) - MAZE_SCALE),
            MAZE_SCALE, pr.Color(0, 0, 255, 170)
        )
    elif simulation_state.player['direction'] == 'DOWN':
        pr.draw_line_ex(
            pr.Vector2(SCALEX(simulation_state.player['x']) - MAZE_SCALE * 1.5, SCALEX(simulation_state.player['y']) + MAZE_SCALE),
            pr.Vector2(SCALEX(simulation_state.player['x']) + MAZE_SCALE * 1.5, SCALEX(simulation_state.player['y']) + MAZE_SCALE),
            MAZE_SCALE, pr.Color(0, 0, 255, 170)
        )

    prev_player = simulation_state.player.copy()

    # Input handling
    if pr.is_key_pressed(pr.KEY_SPACE):
        simulation_state.next_tick(algorithm)  # Placeholder for nextTick function
    if pr.is_key_pressed(pr.KEY_UP):
        simulation_state.player['y'] -= 1  # Placeholder for straight function
    elif pr.is_key_pressed(pr.KEY_LEFT):
        simulation_state.player['direction'] = 'LEFT'  # Placeholder for turnLeft function
    elif pr.is_key_pressed(pr.KEY_RIGHT):
        simulation_state.player['direction'] = 'RIGHT'  # Placeholder for turnRight function
    elif pr.is_key_pressed(pr.KEY_DOWN):
        simulation_state.player['direction'] = 'DOWN'  # Placeholder for turnAround function
    elif pr.is_key_pressed(pr.KEY_P):
        print(f"PATH: {simulation_state.followed_path}")
    elif pr.is_key_pressed(pr.KEY_B):
        print("Visited cells")  # Placeholder for printVisitedCells function

    # Check for invalid player state
    if simulation_state.invalid_player_state():  # Placeholder for invalidPlayerState function
        print("INVALID POSITION")
        simulation_state.player = prev_player

    pr.end_drawing()

# Cleanup
pr.unload_texture(maze_texture)
pr.close_window()