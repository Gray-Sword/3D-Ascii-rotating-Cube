#include <iostream>
#include <cmath>
#include <vector>
#include <utility>
#include <cstring>
#include <chrono>
#include <thread>

using namespace std;

const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 40;
const double FOV = 1.0; 
const double CUBE_SIZE = 0.5;
const double ROTATION_SPEED = 0.05;

vector<pair<int, int>> edges = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

vector<vector<double>> vertices = {
    {-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE},
    { CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE},
    { CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE},
    {-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE},
    {-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE},
    { CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE},
    { CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE},
    {-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE}
};

pair<int, int> projectVertex(const vector<double>& vertex) {
    double x = vertex[0];
    double y = vertex[1];
    double z = vertex[2] + 3; 

    if (z == 0) return {-1, -1}; 

    int screenX = static_cast<int>((x / (FOV * z)) * SCREEN_WIDTH / 2 + SCREEN_WIDTH / 2);
    int screenY = static_cast<int>((y / (FOV * z)) * SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 2);

    return {screenX, screenY};
}

vector<double> rotateVertex(const vector<double>& vertex, double angleX, double angleY, double angleZ) {
    double x = vertex[0];
    double y = vertex[1];
    double z = vertex[2];

    double cosX = cos(angleX), sinX = sin(angleX);
    double cosY = cos(angleY), sinY = sin(angleY);
    double cosZ = cos(angleZ), sinZ = sin(angleZ);

    double xRotY = cosY * x + sinY * z;
    double zRotY = -sinY * x + cosY * z;

    double yRotX = cosX * y - sinX * zRotY;
    double zRotX = sinX * y + cosX * zRotY;

    double xRotZ = cosZ * xRotY - sinZ * yRotX;
    double yRotZ = sinZ * xRotY + cosZ * yRotX;

    return {xRotZ, yRotZ, zRotX};
}

void drawLine(int x1, int y1, int x2, int y2, char screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x1 >= 0 && x1 < SCREEN_WIDTH && y1 >= 0 && y1 < SCREEN_HEIGHT) {
            screenBuffer[y1][x1] = '#';
        }
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void clearScreenBuffer(char screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            screenBuffer[i][j] = ' ';
        }
    }
}

void renderScreenBuffer(char screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            cout << screenBuffer[i][j];
        }
        cout << endl;
    }
}

int main() {
    char screenBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    double angleX = 0, angleY = 0, angleZ = 0;

    while (true) {
        clearScreenBuffer(screenBuffer);

        vector<vector<double>> transformedVertices(vertices.size());

        for (size_t i = 0; i < vertices.size(); i++) {
            transformedVertices[i] = rotateVertex(vertices[i], angleX, angleY, angleZ);
        }

        for (const auto& edge : edges) {
            pair<int, int> p1 = projectVertex(transformedVertices[edge.first]);
            pair<int, int> p2 = projectVertex(transformedVertices[edge.second]);

            int x1 = p1.first, y1 = p1.second;
            int x2 = p2.first, y2 = p2.second;

            if (x1 != -1 && y1 != -1 && x2 != -1 && y2 != -1) {
                drawLine(x1, y1, x2, y2, screenBuffer);
            }
        }

        renderScreenBuffer(screenBuffer);
        this_thread::sleep_for(chrono::milliseconds(50));

        angleX += ROTATION_SPEED;
        angleY += ROTATION_SPEED;
        angleZ += ROTATION_SPEED;

        system("cls");
    }

    return 0;
}
