// sokoban_mac.cpp
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>

using namespace std;

#define CELL_SIZE 50

enum CELL {
    Player,
    BOX,
    WALL,
    TARGET,
    BOXONTARGET,
    DEFAULT
};

// Function to draw the grid using SFML
void draw_grid(const std::vector<std::string>& Grid, sf::RenderWindow& window, sf::Sprite sprites[]) {
    for (size_t i = 0; i < Grid.size(); i++) {
        for (size_t j = 0; j < Grid[i].size(); j++) {
            int x = (int)j * CELL_SIZE;
            int y = (int)i * CELL_SIZE;
            char cell = Grid[i][j];
            switch (cell) {
            case 'S': // Player
                sprites[Player].setPosition(x, y);
                window.draw(sprites[Player]);
                break;
            case 'B': // Box
                sprites[BOX].setPosition(x, y);
                window.draw(sprites[BOX]);
                break;
            case 'R': // Box on target
                sprites[BOXONTARGET].setPosition(x, y);
                window.draw(sprites[BOXONTARGET]);
                break;
            case 'T': // Target
                sprites[TARGET].setPosition(x, y);
                window.draw(sprites[TARGET]);
                break;
            case 'X': // Player on target
                sprites[TARGET].setPosition(x, y);
                window.draw(sprites[TARGET]);
                sprites[Player].setPosition(x, y);
                window.draw(sprites[Player]);
                break;
            case '#': // Wall
                sprites[WALL].setPosition(x, y);
                window.draw(sprites[WALL]);
                break;
            default:  // Empty
                break; 
            }
        }
    }
}

// Function to check victory
bool isgoal(const vector<string>& Grid) {
    for (const auto& row : Grid) {
        for (char cha : row) {
            if (cha == 'T') {
                return false;
            }
        }
    }
    return true;
}

// Function to perform a move
void perform_move(vector<string>& Grid, char move) {
    int m = -1, n = -1;
    int rows = Grid.size();
    int cols = Grid[0].size();
    bool isPlayer = false;
    // Find player's position
    for (int i = 0; i < rows && !isPlayer; i++) {
        for (int j = 0; j < cols; j++) {
            if (Grid[i][j] == 'S' || Grid[i][j] == 'X') {
                m = i;
                n = j;
                isPlayer = true;
                break;
            }
        }
    }

    if (!isPlayer) return;

    int dx = 0, dy = 0;

    switch (move) {
    case 'U':
        dy = -1;
        break;
    case 'D':
        dy = 1;
        break;
    case 'L':
        dx = -1;
        break;
    case 'R':
        dx = 1;
        break;
    default:
        return;
    }

    int row_next = m + dy;
    int col_next = n + dx;

    int box_row = m + 2 * dy;
    int box_col = n + 2 * dx;

    if (row_next < 0 || row_next >= rows || col_next < 0 || col_next >= cols)
        return;

    // Function to check if a cell is free to move
    auto is_free = [&](int row, int col) -> bool {
        if (row < 0 || row >= rows || col < 0 || col >= cols)
            return false;
        char cell = Grid[row][col];
        return (cell == '.' || cell == 'T');
    };

    // Function to check if a cell has a box
    auto has_box = [&](int row, int col) -> bool {
        if (row < 0 || row >= rows || col < 0 || col >= cols)
            return false;
        char cell = Grid[row][col];
        return (cell == 'B' || cell == 'R');
    };

    // Move logic
    if (is_free(row_next, col_next)) {
        // Move player
        if (Grid[m][n] == 'S') {
            Grid[m][n] = '.';
        }
        else if (Grid[m][n] == 'X') {
            Grid[m][n] = 'T';
        }

        if (Grid[row_next][col_next] == '.') {
            Grid[row_next][col_next] = 'S';
        }
        else if (Grid[row_next][col_next] == 'T') {
            Grid[row_next][col_next] = 'X';
        }
    }
    else if (has_box(row_next, col_next)) {
        // Check if the box can be moved
        if (is_free(box_row, box_col)) {
            // Move the box
            if (Grid[box_row][box_col] == '.') {
                Grid[box_row][box_col] = 'B';
            }
            else if (Grid[box_row][box_col] == 'T') {
                Grid[box_row][box_col] = 'R';
            }

            // Move the player
            if (Grid[m][n] == 'S') {
                Grid[m][n] = '.';
            }
            else if (Grid[m][n] == 'X') {
                Grid[m][n] = 'T';
            }

            if (Grid[row_next][col_next] == 'B') {
                Grid[row_next][col_next] = 'S';
            }
            else if (Grid[row_next][col_next] == 'R') {
                Grid[row_next][col_next] = 'X';
            }
        }
    }
}

int main() {
    // Load the grid from file
    vector<string> Grid;
    ifstream infile("big_1.in");

    if (!infile.is_open()) {
        cerr << "Failed to open big_1.in" << endl;
        return -1;
    }

    string line;
    getline(infile, line);
    int row, col;
    istringstream in(line);
    in >> row >> col;

    while (getline(infile, line)) {
        if (!line.empty()) {
            // Ensure that line has exactly 'col' characters
            if ((int)line.size() >= col) {
                Grid.push_back(line.substr(0, col));
            }
            else {
                // Pad the line with '.' if needed
                line += string(col - line.size(), '.');
                Grid.push_back(line);
            }
        }
    }
    infile.close();

    // Create SFML window
    int window_width = col * CELL_SIZE;
    int window_height = row * CELL_SIZE;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Sokoban Simulator");

    // Load textures
    sf::Texture wallTexture, playerTexture, boxTexture, targetTexture, onTargetTexture, backgroundTexture;
    if (!wallTexture.loadFromFile("Wall.jpeg")) {
        cerr << "Failed to load (WALL)" << endl;
        return -1;
    }
    if (!playerTexture.loadFromFile("Player.jpeg")) {
        cerr << "Failed to load (Player)" << endl;
        return -1;
    }
    if (!boxTexture.loadFromFile("Box.jpeg")) {
        cerr << "Failed to load (BOX)" << endl;
        return -1;
    }
    if (!targetTexture.loadFromFile("Target.jpeg")) {
        cerr << "Failed to load (TARGET)" << endl;
        return -1;
    }
    if (!onTargetTexture.loadFromFile("Target_with_box.jpeg")) {
        cerr << "Failed to load (BOXONTARGET)" << endl;
        return -1;
    }
    // Optional: Load background
    bool hasBackground = false;
    if (backgroundTexture.loadFromFile("background.jpeg")) {
        hasBackground = true;
    }

    // Create sprites
    sf::Sprite wallSprite(wallTexture);
    wallSprite.setScale(
        (float)CELL_SIZE / wallTexture.getSize().x,
        (float)CELL_SIZE / wallTexture.getSize().y
    );

    sf::Sprite playerSprite(playerTexture);
    playerSprite.setScale(
        (float)CELL_SIZE / playerTexture.getSize().x,
        (float)CELL_SIZE / playerTexture.getSize().y
    );

    sf::Sprite boxSprite(boxTexture);
    boxSprite.setScale(
        (float)CELL_SIZE / boxTexture.getSize().x,
        (float)CELL_SIZE / boxTexture.getSize().y
    );

    sf::Sprite targetSprite(targetTexture);
    targetSprite.setScale(
        (float)CELL_SIZE / targetTexture.getSize().x,
        (float)CELL_SIZE / targetTexture.getSize().y
    );

    sf::Sprite onTargetSprite(onTargetTexture);
    onTargetSprite.setScale(
        (float)CELL_SIZE / onTargetTexture.getSize().x,
        (float)CELL_SIZE / onTargetTexture.getSize().y
    );

    // Create background sprite
    sf::Sprite backgroundSprite;
    if (hasBackground) {
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            (float)window_width / backgroundTexture.getSize().x,
            (float)window_height / backgroundTexture.getSize().y
        );
    }

    // Store sprites in an array for easier access
    sf::Sprite sprites[DEFAULT];
    sprites[WALL] = wallSprite;
    sprites[Player] = playerSprite;
    sprites[BOX] = boxSprite;
    sprites[TARGET] = targetSprite;
    sprites[BOXONTARGET] = onTargetSprite;

    // Define steps
    string steps = "UUUULLULLULDRDLLLLLLLLRURULLLLRDRDDLUDLLULDDDDLDRRRRRLDDDRRRDDDRRDRRDRRULLLRDDDLLUUURULLDLUUULURRR";
    size_t stepIndex = 0;

    // Timing
    sf::Clock moveClock;
    sf::Time moveDelay = sf::milliseconds(200);

    // Load font for victory message
    sf::Font font;
    bool hasFont = false;
    if (font.loadFromFile("Arial.ttf")) { // Ensure Arial.ttf is available in the project directory
        hasFont = true;
    }
    else {
        cerr << "Failed to load Arial.ttf" << endl;
    }

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Perform move if it's time
        if (stepIndex < steps.length() && moveClock.getElapsedTime() >= moveDelay) {
            char currentMove = steps[stepIndex];
            perform_move(Grid, currentMove);
            stepIndex++;
            moveClock.restart();
        }

        // Draw everything
        window.clear();

        if (hasBackground) {
            window.draw(backgroundSprite);
        }

        draw_grid(Grid, window, sprites);

        window.display();

        // check if 
        if (isgoal(Grid)) {

            if (hasFont) {
                sf::Text text;
                text.setFont(font);
                text.setString("found shortest path of the game.");
                text.setCharacterSize(30);
                text.setFillColor(sf::Color::Red);
                text.setStyle(sf::Text::Bold | sf::Text::Underlined);
                // Center the text
                sf::FloatRect textRect = text.getLocalBounds();
                text.setOrigin(textRect.left + textRect.width / 2.0f,
                               textRect.top  + textRect.height / 2.0f);
                text.setPosition(sf::Vector2f(window_width / 2.0f, window_height / 2.0f));

                // Display the victory message
                window.clear();
                if (hasBackground) {
                    window.draw(backgroundSprite);
                }
                draw_grid(Grid, window, sprites);
                window.draw(text);
                window.display();

                // Wait for user to close the window or press any key
                while (window.isOpen()) {
                    while (window.pollEvent(event)) {
                        if (event.type == sf::Event::Closed)
                            window.close();
                        if (event.type == sf::Event::KeyPressed)
                            window.close();
                    }
                }
            }
            else {
                // If font loading failed, just close the window after displaying victory
                window.close();
            }

            break; // Exit the main loop after victory
        }

        // Optional: Limit frame rate
        window.setFramerateLimit(100);
    }

    return 0;
}