#include "SDL.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>
#include <cstdlib>
#include "SDL_wrapper.h"

int win_height = 800;
int win_width = 1000;

class Cercle {
private:
    int xc;
    int yc;
    int r;
    int sx;
    int sy;
    int red, green, blue;
public:
    Cercle(int pos_x, int pos_y, int t_r, int speed_x, int speed_y, int r_color, int g_color, int b_color) {
        xc = pos_x;
        yc = pos_y;
        r = t_r;
        sx = speed_x;
        sy = speed_y;
        red = r_color;
        green = g_color;
        blue = b_color;
    }

    void creerCercle(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
        for (int w = 0; w < r * 2; w++) {
            for (int h = 0; h < r * 2; h++) {
                int dcx = r - w;
                int dcy = r - h;
                if ((dcx * dcx + dcy * dcy) <= (r * r)) {
                    SDL_RenderDrawPoint(renderer, xc + dcx, yc + dcy);
                }
            }
        }
    }

    void deplacement_Cercle(int win_width, int win_height) {
        xc += sx;
        yc += sy;

        if (xc - r <= 0 || xc + r >= win_width) sx = -sx;
        if (yc - r <= 0 || yc + r >= win_height) sy = -sy;
    }

    bool checkCollision(const Cercle& other) const {
        int dx = other.xc - xc;
        int dy = other.yc - yc;
        int distanceSquared = dx * dx + dy * dy;

        int sumRadii = r + other.r;
        return distanceSquared <= (sumRadii * sumRadii);
    }

    void inverser_vitesse() {
        sx = -sx;
        sy = -sy;
    }
};

bool Overlapping(const Cercle& newCercle, const std::vector<Cercle>& Cercles) {
    for (const auto& Cercle : Cercles) {
        if (newCercle.checkCollision(Cercle)) {
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    SDLWrapper sdlApp("Encapsulation", win_width, win_height);

    std::vector<Cercle> Cercles;
    int frame_count = 0;
    std::srand(time(NULL));
    const int TARGET_FPS = 60;
    const int FRAME_DELAY = 1000 / TARGET_FPS;

    while (sdlApp.running()) {

        int frameStart = SDL_GetTicks();

        if (!sdlApp.processEvents()) break;

        int win_width, win_height;
        sdlApp.getWindowSize(win_width, win_height);


        if (frame_count % 30 == 0) {
            int random_radius = rand() % 25 + 10;
            int random_x, random_y, random_vx, random_vy;
            int random_r, random_g, random_b;
            bool positionValide = false;

            while (!positionValide) {
                random_x = rand() % (win_width - 2 * random_radius) + random_radius;
                random_y = rand() % (win_height - 2 * random_radius) + random_radius;
                random_vx = rand() % 7 - 3;
                random_vy = rand() % 7 - 3;

                random_r = rand() % 255;
                random_g = rand() % 255;
                random_b = rand() % 255;
                Cercle newCercle(random_x, random_y, random_radius, random_vx+1, random_vy+1, random_r, random_g, random_b);

                if (!Overlapping(newCercle, Cercles)) {
                    Cercles.push_back(newCercle);
                    positionValide = true;
                }
            }
        }

        sdlApp.clearScreen(0, 0, 0);

        for (size_t i = 0; i < Cercles.size(); i++) {
            Cercles[i].deplacement_Cercle(win_width, win_height);
            Cercles[i].creerCercle(sdlApp.getRenderer());
            for (size_t j = i + 1; j < Cercles.size(); j++) {
                if (Cercles[i].checkCollision(Cercles[j])) {
                    Cercles[i].inverser_vitesse();
                    Cercles[j].inverser_vitesse();
                }
            }
        }

        sdlApp.updateFPS();
        sdlApp.renderFPS();
        sdlApp.updateScreen();

        int frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }

        frame_count++;
    }
    return 0;
}