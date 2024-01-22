
#include <ftxui/dom/elements.hpp>  // for canvas, Element, separator, hbox, operator|, border
#include <ftxui/screen/screen.hpp>  // for Pixel
#include <memory>   // for allocator, shared_ptr, __shared_ptr_access
#include <string>   // for string, basic_string<
#include <utility>  // for move
#include <vector>   // for vector, __alloc_traits<>::value_type
#include <random>

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Renderer, CatchEvent, Horizontal, Menu, Tab
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/event.hpp"               // for Event
#include "ftxui/component/mouse.hpp"               // for Mouse
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/canvas.hpp"                    // for Canvas
#include "ftxui/screen/color.hpp"  // for Color, Color::Red, Color::Blue, Color::Green, ftxui

#include <bits/stdc++.h> 

using namespace ftxui;

struct Shape {
    int shape[3][4];
    int origin[2];
    int id;
    int rot = 0;

    void rotate() {
        system("kys");
    }
};

struct Shape T_BLOCK = { {{0,1,0,0}, {1,1,1,0}}, {2, 2}, 1}; // can i just hard code the rotations pls
struct Shape O_BLOCK = { {{0,1,1,0}, {0,1,1,0}}, {2, 2}, 2};
struct Shape I_BLOCK = { {{0}, {1,1,1,1}}, {1, 2}, 3};
struct Shape S_BLOCK = { {{0,1,1,0}, {0,0,1,1}}, {1, 3}, 4};
Color colors[4] = {Color::Orange1, Color::Yellow1, Color::SkyBlue1, Color::Red};
struct Shape SHAPES[4] = {T_BLOCK, O_BLOCK, I_BLOCK, S_BLOCK};
struct Shape randomShape() {
    return SHAPES[rand() % 4];
}

class CoolCanvas : public Canvas {
    public:
        CoolCanvas(int x, int y) : Canvas(x, y)
        {

        }
        void drawGrid(int8_t sizex, int8_t sizey, int8_t x, int8_t y) {
            int xAxis = 0;
            int yAxis = 0;
            int xinc = sizex/x;
            int yinc = sizey/y;
            for (xAxis; xAxis <= x; xAxis++) { // draw one more to finish box;
                DrawPointLine(xinc*xAxis, 0, xinc*xAxis, 200, Color::Gold1);
            }
            for (yAxis; yAxis <= y; yAxis++) {
                DrawPointLine(0, yAxis*10, 100, yAxis*10, Color::Red);
            }
        }

        int emptyGrids(int game[][10]) {
                int amm = 0;
                int n = sizeof(game[0]) / sizeof(game[0][0]);

                for (int y = 20; y > 1; y--) {

                    if (std::none_of(game[y], game[y]+n, [](int i) {
                        return i == 0;
                    })) {
                        for (int a = y-1; a > 2; a--) {
                            std::copy(std::begin(game[a]), std::end(game[a]), std::begin(game[a+1]));
                        }
                        y = 20;
                        amm++;
                    }
                }
                
                return amm;
        }

        void filledBlock(int x, int y, int size, Color color) {
            for (int loop = 0; loop < size; loop++)
                DrawBlockLine(x+loop, y, x+loop, y+size, color);
        }


        void drawBlocks() {
            for (int y = 0; y < 20; y++) {
                for (int x = 0; x < 10; x++) 
                    filledBlock(x*10, y*10, 9, Color::Black);
            }
        }

        void drawGameBlocks(int arr[][10]) {
            for (int y = 0; y < 20; y++) {
                for (int x = 0; x < 10; x++) {
                    if (arr[y][x] != 0)
                        filledBlock(x*10, y*10, 9, colors[arr[y][x]-1]);
                }
            }
        }

        void renderPlayerPiece(struct Shape block, int origin[2]) {
            int piece_x = origin[0]*10;
            int piece_y = origin[1]*10;
            for (int x = 0; x < 4; x++) {
                for(int y = 0; y < 2; y++) {
                    if (block.shape[y][x] != 0) {
                        filledBlock(piece_x+x*10, piece_y+y*10, 9, Color::Purple3);
                    }
                }
            }
        }


};

bool pieceHasRoom(struct Shape block, int atOrigin[2], int game[][10]) {
    int start_x = atOrigin[0];
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 4; x++) {
            if (block.shape[y][x] == 1 && game[atOrigin[1]+y][x+start_x] != 0)
                return false;
            if (atOrigin[1]+y > 19)
                return false;
            if (block.shape[y][x] == 1 && (start_x+x > 9 || start_x+x < 0)) 
                return false;
        }
    }

    return true;
}

void dropBlock(struct Shape block, int spot[2], int game[][10]) {
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 4; x++) {
            if (block.shape[y][x] == 1)
                game[y+spot[1]][x+spot[0]] = block.id;
        }
    }
}



int main() {
    int pieceLoc[2] = {5, 0};
    int gameArray[20][10] = {1};
    int frames = 0;
    int score = 0;
    int gameSpeed = 60;

    struct Shape currentShape = randomShape();
    auto screen = ScreenInteractive::FitComponent();

    auto c = CoolCanvas(102, 202);

    auto game_f = Renderer([&] {
        if (!pieceHasRoom(currentShape, pieceLoc, gameArray)) {
            pieceLoc[1]--;
            dropBlock(currentShape, pieceLoc, gameArray);
            score += c.emptyGrids(gameArray) * 100;
            
            pieceLoc[0] = 5; pieceLoc[1] = 0;
            currentShape = randomShape();
            frames = 0;

        }
        if (frames%gameSpeed == 0)
            pieceLoc[1]++;

        
        c.drawBlocks();
        c.drawGameBlocks(gameArray);

        c.renderPlayerPiece(currentShape, pieceLoc);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        frames++;
        screen.PostEvent(Event::Custom);
        return canvas(std::move(c));

    }) | CatchEvent([&](Event event) {

        if (event == Event::ArrowDown) {
            frames=gameSpeed;
            return true;
        } else if (event == Event::ArrowLeft) {
            pieceLoc[0]--;
            if (!pieceHasRoom(currentShape, pieceLoc, gameArray))
                pieceLoc[0]++;
            return true;
        } else if (event == Event::ArrowRight) {
            pieceLoc[0]++;
            if (!pieceHasRoom(currentShape, pieceLoc, gameArray))
                pieceLoc[0]--;
            return true;
        }
        return false;
    });    
    auto component_renderer = Renderer(game_f, [&] {
        return vbox({
            text("SCORE: " + std::to_string(score)),
            game_f->Render(),
        });
    });
    screen.Loop(component_renderer);
}