// Snake Game C++.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "raylib.h"
#include <deque>
#include <raymath.h>

using namespace std;

//Colors
Color green = { 173, 204, 96, 255 };
Color darkGreen = { 43, 51, 24, 255 };

//Make it a grid. Pixelated.
#define cellSize 30
#define cellCount 25
#define offset 75

#define screen_width cellSize*cellCount
#define screen_height cellSize*cellCount

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) return true;
    }
    return false;
}

bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
    Vector2 direction = { 1, 0 };
    bool addSegment = false;

    // Draw the snake rectangles
    void Draw() {
        for (unsigned int i = 0; i < body.size(); i++) {
            DrawRectangleRounded(Rectangle{ (float)offset + (float)body[i].x * (float)cellSize, (float)offset + (float)body[i].y * (float)cellSize, (float)cellSize, (float)cellSize }, 0.5, 6, darkGreen);
        }
    }
    // Updating the snake
    void Update() {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true) addSegment = false;
        else body.pop_back();
    }
    void Reset() {
        body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
        direction = { 1, 0 };
    }
};

class Food {
public:
    Vector2 pos;
    Texture2D texture;

    Food(deque<Vector2> snakeBody) {
        Image image = LoadImage("Images/appleN2.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        pos = GenerateRandomPos(snakeBody);
    }

    ~Food() {
        UnloadTexture(texture);
    }

    void Draw() {
        DrawTexture(texture, offset + pos.x * cellSize, offset + pos.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{ x, y };
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {
        Vector2 pos = GenerateRandomCell();
        while (ElementInDeque(pos, snakeBody)) {
            pos = GenerateRandomCell();
        }
        return pos;
    }
};

class Game {
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;

    void Draw() {
        food.Draw();
        snake.Draw();
    }

    void Update() {
        if (running) {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdge();
            CheckCollisionWithTail();
        }
    }
 
    void CheckCollisionWithFood() {
        if (Vector2Equals(snake.body[0], food.pos)) {
            food.pos = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
        }
    }

    void CheckCollisionWithEdge() {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1 || snake.body[0].y == cellCount || snake.body[0].y == -1) {
            GameOver();
        }
    }

    void CheckCollisionWithTail() {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody)) GameOver();
    }
    
    void GameOver() {
        running = false;
        snake.Reset();
        food.pos = food.GenerateRandomPos(snake.body);
        score = 0;
    }
};

int main()
{
    InitWindow(2*offset + cellSize * cellCount, 2*offset + cellSize * cellCount,  "Snake Game");
    SetTargetFPS(90);

    Game game = Game();

    while (!WindowShouldClose()) {
        BeginDrawing();

        //Updates
        if (eventTriggered(0.2)) game.Update();

        //Snake direction
        if (IsKeyPressed(KEY_W) && game.snake.direction.y != 1) {
            game.snake.direction = { 0, -1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_S) && game.snake.direction.y != -1) {
            game.snake.direction = { 0, 1 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_D) && game.snake.direction.x != -1) {
            game.snake.direction = { 1, 0 };
            game.running = true;
        }
        if (IsKeyPressed(KEY_A) && game.snake.direction.x != 1) {
            game.snake.direction = { -1, 0 };
            game.running = true;
        }

        // Drawing
        ClearBackground(green);
        game.Draw();
        DrawRectangleLinesEx(Rectangle{ (float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10 }, 5, darkGreen);
        DrawText("Snake", offset - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln 