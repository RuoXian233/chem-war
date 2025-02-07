#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <map>

namespace engine {
    void OpenProfilingWindow(const std::string &profilingData) {
        std::ifstream file;
        file.open(profilingData, std::ios::binary);
        float runningTime, freq;
        int count;
        file.read((char *) &runningTime, sizeof(float));
        file.seekg(1, std::ios::cur);
        file.read((char *) &freq, sizeof(float));
        file.seekg(1, std::ios::cur);
        file.read((char *) &count, sizeof(int));
        file.seekg(1, std::ios::cur);
    
        std::vector<float> data;
        float n;
        for (int i = 0; i < count; i++) {
            file.read((char *) &n, sizeof(float));
            data.push_back(n);
            file.seekg(1, std::ios::cur);
        }
        file.close();
    
        float maximum = *std::max_element(data.begin(), data.end());
        float minimum = *std::min_element(data.begin(), data.end());
        float avg = std::accumulate(data.begin(), data.end(), 0) / (data.size() + 0.0f);
        float variance = 0.0f;
        for (int i = 0 ; i < data.size() ; i++) {
            variance += pow(data[i] - avg, 2);
        }
        variance /= data.size();
    
        if (SDL_WasInit(SDL_INIT_EVERYTHING) != SDL_INIT_EVERYTHING) {
            SDL_Init(SDL_INIT_EVERYTHING);
        }
        if (TTF_WasInit() <= 0) {
            TTF_Init();
        }
        SDL_Window *window = SDL_CreateWindow(
            std::format("Profiling Result: {}", profilingData).c_str(), 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            1366, 768, SDL_WINDOW_SHOWN
        );
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
        TTF_Font *f = TTF_OpenFont("./assets/genshin-font.ttf", 24);
        auto lbRuntime_s = std::format("Run Time: {:.2f} sec(s)", runningTime);
        SDL_Surface *lbRuntime = TTF_RenderUTF8_Blended(f, lbRuntime_s.c_str(), { 255, 255, 255, 255 });
        SDL_Texture *lbRuntime_t = SDL_CreateTextureFromSurface(renderer, lbRuntime);
        SDL_Rect lbRuntime_r = { 100, 100, 0, 0 };
        TTF_SizeUTF8(f, lbRuntime_s.c_str(), &lbRuntime_r.w, &lbRuntime_r.h);
    
        auto lbSampleRate_s = std::format("Sample Rate: {:.2f} Hz", 1.0f / freq);
        SDL_Surface *lbSampleRate = TTF_RenderUTF8_Blended(f, lbSampleRate_s.c_str(), { 255, 255, 255, 255 });
        SDL_Texture *lbSampleRate_t = SDL_CreateTextureFromSurface(renderer, lbSampleRate);
        SDL_Rect lbSampleRate_r = { 100, 200, 0, 0 };
        TTF_SizeUTF8(f, lbSampleRate_s.c_str(), &lbSampleRate_r.w, &lbSampleRate_r.h);
    
        auto lbMax_s = std::format("Max: {:.2f} FPS", maximum);
        SDL_Surface *lbMax = TTF_RenderUTF8_Blended(f, lbMax_s.c_str(), { 0, 255, 255, 255 });
        SDL_Texture *lbMax_t = SDL_CreateTextureFromSurface(renderer, lbMax);
        SDL_Rect lbMax_r = { 450, 100, 0, 0 };
        TTF_SizeUTF8(f, lbMax_s.c_str(), &lbMax_r.w, &lbMax_r.h);
    
        auto lbMin_s = std::format("Min: {:.2f} FPS", minimum);
        SDL_Surface *lbMin = TTF_RenderUTF8_Blended(f, lbMin_s.c_str(), { 0, 255, 255, 255 });
        SDL_Texture *lbMin_t = SDL_CreateTextureFromSurface(renderer, lbMin);
        SDL_Rect lbMin_r = { 450, 200, 0, 0 };
        TTF_SizeUTF8(f, lbMin_s.c_str(), &lbMin_r.w, &lbMin_r.h);
    
        auto lbAvg_s = std::format("Avg: {:.2f} FPS", avg);
        SDL_Surface *lbAvg = TTF_RenderUTF8_Blended(f, lbAvg_s.c_str(), { 255, 255, 0, 255 });
        SDL_Texture *lbAvg_t = SDL_CreateTextureFromSurface(renderer, lbAvg);
        SDL_Rect lbAvg_r = { 720, 100, 0, 0 };
        TTF_SizeUTF8(f, lbAvg_s.c_str(), &lbAvg_r.w, &lbAvg_r.h);
    
        auto lbStd_s = std::format("Std: {:.2f} FPS", sqrt(variance));
        SDL_Surface *lbStd = TTF_RenderUTF8_Blended(f, lbStd_s.c_str(), { 255, 255, 0, 255 });
        SDL_Texture *lbStd_t = SDL_CreateTextureFromSurface(renderer, lbStd);
        SDL_Rect lbStd_r = { 720, 200, 0, 0 };
        TTF_SizeUTF8(f, lbStd_s.c_str(), &lbStd_r.w, &lbStd_r.h);

        const float STABLITY_TOLERENCE = 3.0f;
        const int STANDARD_FRAMERATE = 60;
        int stablePointsCount = 0;
        for (auto x : data) {
            if (x + STABLITY_TOLERENCE >= STANDARD_FRAMERATE) {
                stablePointsCount++;
            }
        }
        auto lbStablity_s = std::format("Stb: {:.2f}% ({}~{})", (stablePointsCount + 0.0f) / data.size() * 100, 
            STANDARD_FRAMERATE - STABLITY_TOLERENCE, STANDARD_FRAMERATE);
            
        SDL_Surface *lbStablity = TTF_RenderUTF8_Blended(f, lbStablity_s.c_str(), { 102, 205, 170, 255 });
        SDL_Texture *lbStablity_t = SDL_CreateTextureFromSurface(renderer, lbStablity);
        SDL_Rect lbStablity_r = { 1020, 200, 0, 0 };
        TTF_SizeUTF8(f, lbStablity_s.c_str(), &lbStablity_r.w, &lbStablity_r.h);

        TTF_SetFontSize(f, 16);
        auto tagMax_s = std::format("{:.2f}", maximum);
        SDL_Surface *tagMax = TTF_RenderUTF8_Blended(f, tagMax_s.c_str(), { 255, 255, 255, 255 });
        SDL_Texture *tagMax_t = SDL_CreateTextureFromSurface(renderer, tagMax);
        SDL_Rect tagMax_r = { 32, 350, 0, 0 };
        TTF_SizeUTF8(f, tagMax_s.c_str(), &tagMax_r.w, &tagMax_r.h);
    
        auto tagMin_s = std::format("{:.2f}", minimum);
        SDL_Surface *tagMin = TTF_RenderUTF8_Blended(f, tagMin_s.c_str(), { 255, 255, 255, 255 });
        SDL_Texture *tagMin_t = SDL_CreateTextureFromSurface(renderer, tagMin);
        SDL_Rect tagMin_r = { 32, 684, 0, 0 };
        TTF_SizeUTF8(f, tagMin_s.c_str(), &tagMin_r.w, &tagMin_r.h);
    
        TTF_SetFontSize(f, 18);
        SDL_Surface *tag0 = TTF_RenderUTF8_Blended(f, "FPS", { 255, 255, 255, 255 });
        SDL_Texture *tag0_t = SDL_CreateTextureFromSurface(renderer, tag0);
        SDL_Rect tag0_r = { 82, 316, 0, 0 };
        TTF_SizeUTF8(f, "FPS", &tag0_r.w, &tag0_r.h);
    
        SDL_Surface *tag1 = TTF_RenderUTF8_Blended(f, "time (s)", { 255, 255, 255, 255 });
        SDL_Texture *tag1_t = SDL_CreateTextureFromSurface(renderer, tag1);
        SDL_Rect tag1_r = { 1264, 688, 0, 0 };
        TTF_SizeUTF8(f, "time (s)", &tag1_r.w, &tag1_r.h);
    
        auto FormatSecs = [](int secs) -> std::string {
            int hour = secs / 3600;
            int min = (secs - hour * 3600) / 60;
            int sec = secs - hour * 3600 - min * 60;
            char buf[16] = { 0 };
            sprintf_s(buf, 16, "%02d:%02d:%02d", hour, min, sec);
            return std::string(buf);
        };
    
        auto FillCircle = [renderer](float x, float y, SDL_Color color, int radius = 2) {
            Uint8 r, g, b, a;
            SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            for (float w = 0; w < radius * 2; w += 0.1) {
                for (float h = 0; h < radius * 2; h += 0.1) {
                    float dx = radius - w;
                    float dy = radius - h;
                    if ((dx * dx + dy * dy) <= (radius * radius)) {
                        SDL_RenderDrawPointF(renderer, dx + x, y + dy);
                    }
                }
            }
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
        };
    
        constexpr int TIME_TAGS_COUNT = 7;
        constexpr int SCALING_BOUND = 320;
        TTF_SetFontSize(f, 16);
        std::string timetags_s[TIME_TAGS_COUNT] { "" };
        auto period = (float) (runningTime / TIME_TAGS_COUNT);
        timetags_s[0] = FormatSecs(0);
        // timetags_s[6] = FormatSecs((int) runningTime);
        for (int i = 1; i < TIME_TAGS_COUNT; i++) {
            timetags_s[i] = FormatSecs(period * i);
        }
    
        SDL_Surface *timetags[TIME_TAGS_COUNT] { 0 };
        SDL_Texture *timetags_t[TIME_TAGS_COUNT] { 0 };
        SDL_Rect timetags_r[TIME_TAGS_COUNT] { { 0 } };
        for (int i = 0; i < TIME_TAGS_COUNT; i++) {
            timetags[i] = TTF_RenderUTF8_Blended(f, timetags_s[i].c_str(), { 216, 191, 216, 255 });
            timetags_t[i] = SDL_CreateTextureFromSurface(renderer, timetags[i]);
            TTF_SizeUTF8(f, timetags_s[i].c_str(), &timetags_r[i].w, &timetags_r[i].h);
            timetags_r[i].x = 100 + (1300 - 50) / TIME_TAGS_COUNT * i;
            timetags_r[i].y = 716;
        }
    
        std::vector<std::pair<float, float>> points;
        // Calculate point position
        // y: max = 692, min = 359
        // x: max = 1250, min = 100
        if (data.size() > SCALING_BOUND) {
            // Large data scaling procedure
        } else {
            for (int i = 0; i < data.size(); i++) {
                float x = 100 + (i + 0.0f) / data.size() * (1250 - 100);
                float y;
                if (maximum != minimum) {
                    y = 692 - (data[i] - minimum) / (maximum - minimum) * (692 - 359);                
                } else {
                    y = 359;
                }
                points.push_back(std::make_pair(x, y));
            }
        }
        std::sort(points.begin(), points.end(), [](auto v1, auto v2) { return v1.first < v2.first; });
    
        bool running = true;
        bool mouseOnGraph = false;
        SDL_Event event;
        const int mouseCollideTolerence = 5;
        const float mouseCollideDistanceRange = 3.2f;
        int mouseX, mouseY;
        std::map<int, float> candidates;
        std::vector<int> janks;

        for (int i = 0; i < 10; i++) {

        }

        TTF_SetFontSize(f, 24);
        while (running) {
            SDL_RenderClear(renderer);
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                } else if (event.type == SDL_MOUSEMOTION) {
                    candidates.clear();
                    int x = event.motion.x, y = event.motion.y;
                    mouseX = x, mouseY = y;
                    // float rx = (x - 100) / 1250.0f * points.size();
                    
                    if (x > 100) {
                        int start = x < 100 + 1250 / 2 ? 0 : points.size() / 2;
                        int end = x < 100 + 1250 / 2 ? points.size() / 2 : points.size();
                        // start = start < 0 ? 0 : start;
                        // end = end >= points.size() ? points.size() - 1 : end;P
                        for (int i = start; i < end; i++) {
                            float dist = sqrt(pow(points[i].first - x, 2.0) + pow(points[i].second - y, 2.0));
                            if (dist <= mouseCollideDistanceRange) {
                                candidates.insert(std::make_pair(i, dist));
                            }
                        }
                    }
                    mouseOnGraph = !candidates.empty();
                }
            }
    
            auto mousePosIndicator_s = std::format("Mouse: ({}, {})", mouseX, mouseY);
            SDL_Surface *mousePosIndicator = TTF_RenderUTF8_Blended(f, mousePosIndicator_s.c_str(), { 107, 142, 35, 255 });
            SDL_Texture *mousePosIndicator_t = SDL_CreateTextureFromSurface(renderer, mousePosIndicator);
            SDL_Rect mousePosIndicator_r = { 1100, 596, 0, 0 };
            TTF_SizeUTF8(f, mousePosIndicator_s.c_str(), &mousePosIndicator_r.w, &mousePosIndicator_r.h);
            SDL_RenderCopy(renderer, mousePosIndicator_t, nullptr, &mousePosIndicator_r);
    
            if (mouseOnGraph) {
                for (auto [k, v] : candidates) {
                    FillCircle(points[k].first, points[k].second, { 144, 238, 144, 255 }, 5);
                }
    
                if (candidates.size() != 0) {
                    int i;
                    float xVal, yVal, d0 = INFINITY;
                    if (candidates.size() != 1) {
                        for (auto [k, v] : candidates) {
                            if (v < d0) {
                                d0 = v;
                                i = k;
                            }
                        }
                    } else {
                        for (auto [k, v] : candidates) {
                            i = k;
                        }
                    }
                    xVal = (i + 0.0f) / data.size() * runningTime, yVal = data[i];
                    auto pointIndicator_s = std::format("#{}: ({:.1f}s, {:.2f} FPS)", i, xVal, yVal);
                    SDL_Surface *pointIndicator = TTF_RenderUTF8_Blended(f, pointIndicator_s.c_str(), { 255, 192, 203, 255 });
                    SDL_Texture *pointIndicator_t = SDL_CreateTextureFromSurface(renderer, pointIndicator);
                    SDL_Rect pointIndicator_r = { 1000, 532, 0, 0 };
                    TTF_SizeUTF8(f, pointIndicator_s.c_str(), &pointIndicator_r.w, &pointIndicator_r.h);
                    pointIndicator_r.x = mousePosIndicator_r.x + mousePosIndicator_r.w - pointIndicator_r.w;
                    SDL_RenderCopy(renderer, pointIndicator_t, nullptr, &pointIndicator_r); 
    
                    SDL_FreeSurface(pointIndicator);
                    SDL_DestroyTexture(pointIndicator_t);
                }
            }
    
            SDL_SetRenderDrawColor(renderer, 3, 22, 52, 255);
            SDL_RenderCopy(renderer, lbRuntime_t, nullptr, &lbRuntime_r);
            SDL_RenderCopy(renderer, lbSampleRate_t, nullptr, &lbSampleRate_r);
            SDL_RenderCopy(renderer, lbMax_t, nullptr, &lbMax_r);
            SDL_RenderCopy(renderer, lbMin_t, nullptr, &lbMin_r);
            SDL_RenderCopy(renderer, lbAvg_t, nullptr, &lbAvg_r);
            SDL_RenderCopy(renderer, lbStd_t, nullptr, &lbStd_r);
            SDL_RenderCopy(renderer, lbStablity_t, nullptr, &lbStablity_r);
    
            // Draw a framerate graph
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, 100, 700, 100, 350);
            SDL_RenderDrawLine(renderer, 100, 700, 1250, 700);
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
            SDL_RenderDrawLine(renderer, 90, 359, 98, 359);
            SDL_RenderDrawLine(renderer, 90, 692, 98, 692);
            SDL_SetRenderDrawColor(renderer, 3, 22, 52, 255);
    
            SDL_RenderCopy(renderer, tagMax_t, nullptr, &tagMax_r);
            SDL_RenderCopy(renderer, tag0_t, nullptr, &tag0_r);
            SDL_RenderCopy(renderer, tag1_t, nullptr, &tag1_r);
            SDL_RenderCopy(renderer, tagMin_t, nullptr, &tagMin_r);
            SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
            for (int i = 0; i < TIME_TAGS_COUNT; i++) {
                SDL_RenderCopy(renderer, timetags_t[i], nullptr, &timetags_r[i]);
                SDL_RenderDrawLine(renderer, timetags_r[i].x + timetags_r[i].w / 2, timetags_r[i].y - 3, timetags_r[i].x + timetags_r[i].w / 2, timetags_r[i].y - 14);
            }
    
            SDL_SetRenderDrawColor(renderer, 255, 192, 203, 255);
            for (auto [x, y] : points) {
                // SDL_RenderDrawPointF(renderer, x, y);
                // FillCircle(x, y, { 144, 238, 144, 255 });
            }
            for (int i = 0; i < points.size(); i++) {
                if (i != points.size() - 1) {
                    SDL_RenderDrawLineF(renderer, points[i].first, points[i].second, points[i + 1].first, points[i + 1].second);
                }
            }
            SDL_SetRenderDrawColor(renderer, 3, 22, 52, 255);
    
            SDL_RenderPresent(renderer);
            SDL_FreeSurface(mousePosIndicator);
            SDL_DestroyTexture(mousePosIndicator_t);
        }
    
        SDL_FreeSurface(lbRuntime);
        SDL_FreeSurface(lbSampleRate);
        SDL_FreeSurface(lbMax);
        SDL_FreeSurface(lbMin);
        SDL_FreeSurface(lbAvg);
        SDL_FreeSurface(lbStd);
        SDL_FreeSurface(tag0);
        SDL_FreeSurface(tag1);
        SDL_FreeSurface(tagMax);
        SDL_FreeSurface(tagMin);
        SDL_FreeSurface(lbStablity);
        SDL_DestroyTexture(lbRuntime_t);
        SDL_DestroyTexture(lbSampleRate_t);
        SDL_DestroyTexture(lbMax_t);
        SDL_DestroyTexture(lbMin_t);
        SDL_DestroyTexture(lbAvg_t);
        SDL_DestroyTexture(lbStd_t);
        SDL_DestroyTexture(tag0_t);
        SDL_DestroyTexture(tag1_t);
        SDL_DestroyTexture(tagMin_t);
        SDL_DestroyTexture(tagMax_t);
        SDL_DestroyTexture(lbStablity_t);
        for (int i = 0; i < TIME_TAGS_COUNT; i++) {
            SDL_DestroyTexture(timetags_t[i]);
            SDL_FreeSurface(timetags[i]);
        }
        TTF_CloseFont(f);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }
}

