# Bottle Catcher Game

## Overview
This project is a 2D bottle-catching game developed using OpenGL and C++ as part of our computer graphics coursework. Players control a bowl to catch falling bottles, aiming to achieve a high score.

Developed by **Ahmed Hassan**, **Hesham Abdelgawad**, and **Abdelaziz Serour** under the supervision of **Dr. Amin Shokry**, **Eng. Yasser**, and **Eng. Omnia**.

---

## Features

- **Player Controls:**
  - Use the arrow keys or mouse to move the bowl horizontally within the game window.

- **Dynamic Gameplay:**
  - Bottles fall from random positions and increase in speed as the game progresses.
  - Catch bottles to score points; missing ends the game.

- **Game States:**
  - **Playing:** Active gameplay where bottles fall, and scores increase.
  - **Game Over (Lose):** Triggered if a bottle is missed.
  - **Game Over (Win):** Triggered when the player achieves a score of 100.

- **Visual Elements:**
  - The bowl is represented as a red semi-circle.
  - Bottles are green rectangles with a neck for added realism.
  - Score updates and end-game messages displayed dynamically on the screen.

- **Restart Option:**
  - Players can restart the game after it ends by pressing the `R` key.

---

## How It Works

### Window Setup
- The game runs in an 800x600 OpenGL window.
- The viewport and orthographic projection dynamically adjust for window resizing.

### Game Logic
- Bottles are managed as objects in a vector, with attributes like position, state, and dimensions.
- Collision detection ensures accuracy when checking if the bowl catches bottles.

### Rendering
- **Bowl:** Drawn as a red semi-circle using triangle fans.
- **Bottles:** Rendered as quads for the body and neck.
- **Text:** Dynamic score and messages rendered using bitmap and stroke text functions.

### Input Handling
- Arrow keys or mouse control the bowl's horizontal movement.
- Pressing the `R` key restarts the game after a Game Over.

### Game Flow
- Bottles spawn periodically at random horizontal positions.
- Speed increases over time for an engaging gameplay experience.

---

## How to Run

1. Clone the repository or download the source files.
2. Ensure **OpenGL** and **GLUT** libraries are installed on your system.
3. Compile the code using a C++ compiler:
   ```bash
   g++ -o BottleCatcher BottleCatcher.cpp -lGL -lGLU -lglut
    bash```