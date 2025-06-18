import json
import matplotlib.pyplot as plt

def load_walls(filename):
    with open(filename, 'r') as f:
        return json.load(f)

def draw_walls(walls):
    plt.figure(figsize=(8,8)) 
    ax = plt.gca()
    
    for wall in walls:
        x_values = [wall['x1'], wall['x2']]
        y_values = [wall['y1'], wall['y2']]
        
        # Draw a thick black line for each wall segment
        ax.plot(x_values, y_values, color='black', linewidth=3)
        
        # Optionally, add length text at midpoint
        mid_x = (wall['x1'] + wall['x2']) / 2
        mid_y = (wall['y1'] + wall['y2']) / 2
        length = wall.get('length', 0)
        ax.text(mid_x, mid_y, f"{length:.1f} mm", fontsize=8, color='red')
    
    ax.set_aspect('equal', adjustable='box')
    ax.set_title('PlanMorph Wall Drawing')
    plt.xlabel('X (mm)')
    plt.ylabel('Y (mm)')
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    walls = load_walls('walls.json')
    if not walls:
        print("No walls found in walls.json")
    else:
        draw_walls(walls)
