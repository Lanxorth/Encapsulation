#include "SDL_wrapper.h"
#include <iostream>
#include <string>
#include "SDL_ttf.h"
#include "SDL.h"

SDLWrapper::SDLWrapper(const char* title, int win_width, int win_height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0 || TTF_Init() != 0) {
        std::cout << "Erreur d'initialisation de la SDL : " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }


    fenetre = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_height, SDL_WINDOW_RESIZABLE);

    if (fenetre == NULL) {
        std::cout << "Erreur lors de la création de la fenêtre : " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        std::cout << "Erreur lors de la création du renderer : " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    lastFrameTime = SDL_GetTicks();
    framecount = 0;
    texW = 0;
    texH = 0;

    font = TTF_OpenFont("./OpenSans-Regular.ttf", 24);
    if (!font) {
        std::cerr << "Erreur lors du chargement de la police : " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    fpsTexture = nullptr;
    isRunning = true;
}

SDLWrapper::~SDLWrapper() {
    SDL_DestroyTexture(fpsTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    TTF_Quit();
    SDL_Quit();
}

bool SDLWrapper::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
    }
    return isRunning;
}

void SDLWrapper::clearScreen(int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
}

void SDLWrapper::updateScreen() {
    SDL_RenderPresent(renderer);
}

SDL_Renderer* SDLWrapper::getRenderer() {
    return renderer;
}

void SDLWrapper::getWindowSize(int& win_width, int& win_height) {
    SDL_GetWindowSize(fenetre, &win_width, &win_height);
}

bool SDLWrapper::running() const {
    return isRunning;
}

SDL_Texture* SDLWrapper::renderText(const std::string& message, SDL_Color color, int fontSize) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, message.c_str(), color);
    if (!surface) {
        std::cerr << "Failed to create surface: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

float SDLWrapper::calculateFPS() {
    int currentTime = SDL_GetTicks();
    framecount++;

    if (currentTime - lastFrameTime >= 1000) {
        float fps = framecount / ((currentTime - lastFrameTime) / 1000.0f);
        lastFrameTime = currentTime;
        framecount = 0;
        return fps;
    }
    return -1.0f;
}

void SDLWrapper::updateFPS() {
    float fps = calculateFPS();
    if (fps >= 0) {
        if (fpsTexture) {
            SDL_DestroyTexture(fpsTexture);
        }

        SDL_Color white = { 255, 255, 255, 255 };
        fpsTexture = renderText("FPS: " + std::to_string(static_cast<int>(fps)), white, 24);

        if (fpsTexture) {
            SDL_QueryTexture(fpsTexture, nullptr, nullptr, &texW, &texH);
        }
    }
}

void SDLWrapper::renderFPS() {
    if (fpsTexture) {
        SDL_Rect dstRect = { 10, 10, texW, texH };
        SDL_RenderCopy(renderer, fpsTexture, nullptr, &dstRect);
    }
}