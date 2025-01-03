#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath> // For M_PI
#include <random>
#include <chrono>
#include <thread>

#define _USE_MATH_DEFINES // To ensure M_PI is defined
#include <cmath>

std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

// Window dimensions
const int WIDTH = 800;
const int HEIGHT = 600;

int currentWidth = WIDTH;
int currentHeight = HEIGHT;

// Player (bowl) properties
float playerPosX = 0.0f;
float playerWidth = 0.2f; // Adjusted for better appearance

// Bottle properties
float bottleWidth = 0.05f;
float speed = 0.01f;
int score = 0;

// Base speed (unchanged)
const float baseSpeed = 0.05f;

// Scaled speed (depends on window size)
float scaledSpeed = baseSpeed;

// Define game states
enum GameState {
    PLAYING,
    GAME_OVER_LOSE,
    GAME_OVER_WIN
};

GameState currentState = PLAYING; // Initial game state

// Structure to represent a bottle
struct Bottle {
    float posX;
    float posY;
    bool active;
};

std::vector<Bottle> bottles;

// Function to handle special key input (arrow keys)
void key_callback(int key, int x, int y) {
    if (currentState != PLAYING)
        return; // Ignore input if not playing

    if (key == GLUT_KEY_LEFT) {
        if (playerPosX - playerWidth / 2 > -1.0f) {
            playerPosX -= scaledSpeed;
        }
    }
    if (key == GLUT_KEY_RIGHT) {
        if (playerPosX + playerWidth / 2 < 1.0f) {
            playerPosX += scaledSpeed;
        }
    }
}

// Function to handle normal key input
void keyboard_callback(unsigned char key, int x, int y) {
    if (currentState == GAME_OVER_LOSE || currentState == GAME_OVER_WIN) {
        if (key == 'r' || key == 'R') {
            // Reset game variables
            score = 0;
            playerPosX = 0.0f;
            bottles.clear();
            currentState = PLAYING;
        }
    }
}

void mouse_motion_callback(int x, int y) {
    if (currentState != PLAYING)
        return; // Ignore input if not playing

    // Calculate the aspect ratio
    float aspectRatio = static_cast<float>(currentWidth) / currentHeight;

    // Map mouse X position to OpenGL coordinate system
    float normalizedX = (static_cast<float>(x) / currentWidth) * 2.0f - 1.0f;

    // Adjust for aspect ratio to align correctly
    if (aspectRatio >= 1.0f) {
        // Wider than tall: No adjustment needed
        playerPosX = normalizedX;
    } else {
        // Taller than wide: Adjust normalizedX based on aspect ratio
        playerPosX = normalizedX / aspectRatio;
    }

    // Ensure the catcher stays within screen boundaries
    if (playerPosX - playerWidth / 2 < -1.0f) {
        playerPosX = -1.0f + playerWidth / 2;
    }
    if (playerPosX + playerWidth / 2 > 1.0f) {
        playerPosX = 1.0f - playerWidth / 2;
    }
}

// Function to check collision between two circles
bool checkCollision(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    float distanceSquared = dx * dx + dy * dy;
    float radiusSum = r1 + r2;
    return distanceSquared < (radiusSum * radiusSum);
}

// Function to draw a bowl using a semi-circle
void drawBowl(float x, float y, float width, float height) {
    int numSegments = 50; // Increase for smoother curve
    float radius = width / 2.0f;
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the bowl

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // Center of the fan (bottom center of the bowl)
        for(int i = 0; i <= numSegments; i++) {
            float angle = M_PI * i / numSegments; // From 0 to PI
            float dx = radius * cos(angle);
            float dy = radius * sin(angle);
            glVertex2f(x + dx, y + dy);
        }
    glEnd();
}

// Function to draw a bottle using quads
void drawBottle(float x, float y, float width, float height) {
    // Body of the bottle
    glColor3f(0.0f, 1.0f, 0.0f); // Green color for the bottle

    glBegin(GL_QUADS);
        glVertex2f(x - width / 2, y);
        glVertex2f(x + width / 2, y);
        glVertex2f(x + width / 2, y + height * 0.7f);
        glVertex2f(x - width / 2, y + height * 0.7f);
    glEnd();

    // Neck of the bottle
    glBegin(GL_QUADS);
        glVertex2f(x - width * 0.2f, y + height * 0.8f);
        glVertex2f(x + width * 0.2f, y + height * 0.8f);
        glVertex2f(x + width * 0.2f, y + height);
        glVertex2f(x - width * 0.2f, y + height);
    glEnd();
}

// Function to draw bitmap text (for the score and prompts)
void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Function to draw stroke text (big bold font)
void drawStrokeText(const char* text, float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, scale); // Scale the text

    for (const char* c = text; *c != '\0'; c++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }

    glPopMatrix();
}

// Function to display objects (player, bottles, score)
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (currentState == PLAYING) {
        // Draw the player (bowl)
        float bowlWidth = playerWidth;  // Adjust as needed
        float bowlHeight = playerWidth * 0.5f; // Adjust aspect ratio
        drawBowl(playerPosX, -0.9f, bowlWidth, bowlHeight);

        // Draw the falling bottles
        for (auto it = bottles.begin(); it != bottles.end(); ) {
            if ((*it).active) {
                float bottleHeight = bottleWidth * 3.0f; // Adjust as needed
                drawBottle((*it).posX, (*it).posY - 0.1f, bottleWidth, bottleHeight);

                // Collision detection using bounding circles
                float bowlRadius = playerWidth / 2.0f;
                float bottleRadius = bottleWidth / 2.0f;
                float bottleCenterX = (*it).posX;
                float bottleCenterY = (*it).posY + (bottleWidth * 2.0f) * 0.4f; // Approximate center

                if (checkCollision(playerPosX, -0.9f + bowlHeight / 2.0f, bowlRadius,
                                  bottleCenterX, bottleCenterY, bottleRadius)) {
                    score++;
                    speed += 0.0005;
                    (*it).active = false; // Deactivate the bottle

                    // Check for win condition
                    if (score >= 100) {
                        currentState = GAME_OVER_WIN;
                        break; // Exit the loop as the game is won
                    }
                }

                // Make the bottle fall
                (*it).posY -= speed;

                if ((*it).posY < -1.0f) {
                    // Bottle missed
                    currentState = GAME_OVER_LOSE;
                    break; // Exit the loop as the game is over
                }

                ++it;
            } else {
                // Remove inactive bottles
                it = bottles.erase(it);
            }
        }

        // Draw the score
        glColor3f(1.0f, 1.0f, 1.0f); // White color for the text
        char scoreText[20];
        sprintf(scoreText, "Score: %d", score);
        drawText(scoreText, -0.95f, 0.9f);
    }
    if (currentState == GAME_OVER_LOSE) {
        // Display "LOSER" in big bold white font
        glColor3f(1.0f, 1.0f, 1.0f); // White color for text
        drawStrokeText("LOSER", -1.02f, 0.2f, 0.005f); // Adjust position and scale as needed

        // Display final score
        glColor3f(1.0f, 1.0f, 1.0f); // White color for score
        char finalScoreText[30];
        sprintf(finalScoreText, "Final Score: %d", score);
        drawText(finalScoreText, -0.1f, 0.0f);

        // Prompt to restart
        glColor3f(0.0f, 1.0f, 0.0f); // Green color for restart prompt
        const char* restartText = "Press 'r' to Restart";
        drawText(restartText, -0.15f, -0.2f);
    }
    if (currentState == GAME_OVER_WIN) {
        // Display "SIGMA" in big bold white font
        glColor3f(1.0f, 1.0f, 1.0f); // White color for text
        drawStrokeText("SIGMA", -0.9f, 0.2f, 0.005f); // Adjust position and scale as needed

        // Display final score
        glColor3f(1.0f, 1.0f, 1.0f); // White color for score
        char finalScoreText[30];
        sprintf(finalScoreText, "Final Score: %d", score);
        drawText(finalScoreText, -0.1f, 0.0f);

        // Prompt to restart
        glColor3f(0.0f, 1.0f, 0.0f); // Green color for restart prompt
        const char* restartText = "Press 'r' to Restart";
        drawText(restartText, -0.15f, -0.2f);
    }

    glutSwapBuffers();
}

// Function to add a new bottle
void addBottle() {
    Bottle newBottle;
    newBottle.posX = static_cast<float>(rng()) / RAND_MAX * 2.0f - 1.0f;
    newBottle.posY = 1.0f;

    // Ensure the bottle stays within the screen boundaries
    if (newBottle.posX - bottleWidth / 2 < -1.0f) {
        newBottle.posX = -1.0f + bottleWidth / 2;
    }
    if (newBottle.posX + bottleWidth / 2 > 1.0f) {
        newBottle.posX = 1.0f - bottleWidth / 2;
    }

    newBottle.active = true;
    bottles.push_back(newBottle);
}

// Function to update game objects (called every frame)
void update(int value) {
    if (currentState == PLAYING) {
        glutPostRedisplay(); // Redraw the scene
    }
    glutTimerFunc(16, update, 0); // Call the update function every 16 ms (~60 FPS)
}

// Timer function to spawn bottles every 2 seconds
void spawnBottle(int value) {
    if (currentState == PLAYING) {
        addBottle();
    }
    glutTimerFunc(2000 / (2 / 100 * score + 1), spawnBottle, 0); // Spawn a bottle every 2000 ms
}


void reshape(int newWidth, int newHeight) {
    // Prevent division by zero
    if (newHeight == 0)
        newHeight = 1;

    // Update global variables for current window size
    currentWidth = newWidth;
    currentHeight = newHeight;

    // Recalculate scaledSpeed based on the aspect ratio
    scaledSpeed = baseSpeed * static_cast<float>(newWidth) / WIDTH;

    // Set the viewport to cover the entire window
    glViewport(0, 0, newWidth, newHeight);

    // Calculate the new aspect ratio
    float aspectRatio = static_cast<float>(newWidth) / newHeight;

    // Adjust the projection to maintain the aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (aspectRatio >= 1.0f) {
        // Wider than tall
        gluOrtho2D(-aspectRatio, aspectRatio, -1.0f, 1.0f);
    } else {
        // Taller than wide
        gluOrtho2D(-1.0f, 1.0f, -1.0f / aspectRatio, 1.0f / aspectRatio);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



// Function to initialize OpenGL settings
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background to black
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f); // Set 2D orthogonal projection

    // Initialize random seed
    srand(static_cast<unsigned int>(time(0)));
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Double buffering for smooth rendering
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Bottle Catcher");

    glutDisplayFunc(display); // Register display function
    glutSpecialFunc(key_callback); // Register special key input function (arrow keys)
    glutKeyboardFunc(keyboard_callback); // Register normal key input function ('r' key)
    glutPassiveMotionFunc(mouse_motion_callback); // Register mouse motion callback
    glutReshapeFunc(reshape); // Register reshape callback
    glutTimerFunc(16, update, 0); // Set up a timer to call the update function every 16 ms (~60 FPS)
    glutTimerFunc(0, spawnBottle, 0); // Set up a timer to spawn bottles every 2000 ms

    init(); // Initialize OpenGL settings

    glutMainLoop(); // Start the GLUT main loop

    return 0;
}
