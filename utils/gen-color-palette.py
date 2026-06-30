import os
import random
import numpy as np
import tkinter as tk

def generate_rgb_palette(num_colors=128):
    """Generates a visually cohesive RGB color palette."""
    palette = []

    for i in range(num_colors):
        angle = 2.0 * np.pi * i / num_colors
        
        # np.cos returns -1.0 to 1.0. 
        # Shift and scale to 0.0 to 1.0 range so values are never negative.
        r_float = (np.cos(angle) + 1.0) / 2.0
        g_float = (np.cos(0.5 * angle) + 1.0) / 2.0
        b_float = (np.cos(1.5 * angle) + 1.0) / 2.0

        # Cast to proper 0-255 RGB integers
        r = int(r_float * 256)
        g = int(g_float * 256)
        b = int(b_float * 256)

        palette.append((r, g, b))

    return palette


def save_palette_to_file(palette, filename="palette.txt"):
    """Writes the RGB and Hex color codes to a text file."""
    with open(filename, "w") as f:
        # Fixed: Added the missing 'f' prefix to parse {len(palette)} variables
        f.write(f"ncolors={len(palette)}\n")
        f.write("#  R    G    B\n")

        for i, color in enumerate(palette, 1):
            # Formats RGB columns nicely aligned using spaces
            f.write(f"  {color[0]:3d}  {color[1]:3d}  {color[2]:3d}\n")

    print(
        f"Success: Palette successfully saved to '{os.path.abspath(filename)}'"
    )


def display_palette_window(palette):
    """Opens a Tkinter window to display a dense 128-color map bar."""
    root = tk.Tk()
    root.title("RGB Color Palette Map")
    root.geometry("800x150")

    # Use a Canvas to handle 128 colors smoothly instead of 128 individual Frame widgets
    canvas = tk.Canvas(root, bg="white")
    canvas.pack(fill=tk.BOTH, expand=True)

    def draw_palette(event=None):
        canvas.delete("all")
        width = canvas.winfo_width()
        height = canvas.winfo_height()
        
        num_colors = len(palette)
        # Calculate width of each fine color stripe
        stripe_width = width / num_colors

        for i, color in enumerate(palette):
            hex_code = f"#{color[0]:02X}{color[1]:02X}{color[2]:02X}"
            x0 = i * stripe_width
            x1 = (i + 1) * stripe_width
            
            # Draw color band
            canvas.create_rectangle(x0, 0, x1, height, fill=hex_code, outline=hex_code)

    # Re-draws the gradient perfectly whenever you resize the window
    canvas.bind("<Configure>", draw_palette)
    root.mainloop()


# --- Main Execution ---
if __name__ == "__main__":
    # 1. Generate 128 colors
    my_palette = generate_rgb_palette(128)

    # 2. Save colors to a text file
    save_palette_to_file(my_palette, "palette.txt")

    # 3. Launch the visual window
    display_palette_window(my_palette)

