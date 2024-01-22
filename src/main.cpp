
#include <ftxui/dom/elements.hpp>  // for canvas, Element, separator, hbox, operator|, border
#include <ftxui/screen/screen.hpp>  // for Pixel
#include <memory>   // for allocator, shared_ptr, __shared_ptr_access
#include <string>   // for string, basic_string
#include <utility>  // for move
#include <vector>   // for vector, __alloc_traits<>::value_type
 
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Renderer, CatchEvent, Horizontal, Menu, Tab
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/event.hpp"               // for Event
#include "ftxui/component/mouse.hpp"               // for Mouse
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/canvas.hpp"                    // for Canvas
#include "ftxui/screen/color.hpp"  // for Color, Color::Red, Color::Blue, Color::Green, ftxui
struct Shape {
    int shape[2][4];
    int origin[2];
};

struct Shape T_BLOCK = { {{0,1,0,0}, {1,1,1,0}}, {2, 2} };
struct Shape O_BLOCK = { {{0,1,1,0}, {0,1,1,0}}, {2, 2} };


using namespace ftxui;


void drawGrid(Canvas &canvas, int8_t sizex, int8_t sizey, int8_t x, int8_t y) {
    int xAxis = 0;
    int yAxis = 0;
    int xinc = sizex/x;
    int yinc = sizey/y;
    for (xAxis; xAxis <= x; xAxis++) { // draw one more to finish box;
        canvas.DrawPointLine(xinc*xAxis, 0, xinc*xAxis, 200, Color::Gold1);
    }
    for (yAxis; yAxis <= y; yAxis++) {
        canvas.DrawPointLine(0, yAxis*10, 100, yAxis*10, Color::Red);
    }
}


void filledBlock(Canvas &canvas, int x, int y, int size, Color color) {
    for (int loop = 0; loop < size; loop++)
        canvas.DrawBlockLine(x+loop, y, x+loop, y+size, color);
}

void drawBlocks(Canvas &canvas) {
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 10; x++) 
            filledBlock(canvas, x*10, y*10, 9, Color::Black);
    }
}

void drawGameBlocks(int arr[][10], Canvas &canvas) {
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 10; x++) {
            if (arr[y][x] != 0)
                filledBlock(canvas, x*10, y*10, 9, Color::BlueLight);
        }
    }
}

void renderPlayerPiece(Canvas &canvas, struct Shape block, int origin[2]) {
    // calculate origin based on block origin
    // must render origin block at x, y
    int piece_x = origin[0]*10;
    int piece_y = origin[1]*10;
    for (int x = 0; x < 4; x++) {
        for(int y = 0; y < 2; y++) {
            if (block.shape[y][x] != 0) {
                filledBlock(canvas, piece_x+x*10, piece_y+y*10, 9, Color::Purple3);
            }
        }
    }
}


int main() {
    int pieceLoc[2] = {5, 1};
    int gameArray[20][10] = { 0 };

    auto c = Canvas(105, 205); // this doesnt fit inside of the terminal, probably do alot of math later

    auto game_f = Renderer([&] {
        drawBlocks(c);
        drawGameBlocks(gameArray, c);

        renderPlayerPiece(c, O_BLOCK, pieceLoc);
        return canvas(std::move(c));
    }) | CatchEvent([&](Event event) {

        if (event == Event::ArrowDown) {
            pieceLoc[1]++;
            return true;
        } else if (event == Event::ArrowLeft) {
            pieceLoc[0]--;
            return true;
        } else if (event == Event::ArrowRight) {
            pieceLoc[0]++;
            return true;
        }
        return false;
    });

    
    auto component_renderer = Renderer(game_f, [&] {
        return vbox({
            text(std::to_string(pieceLoc[0]) + "," + std::to_string(pieceLoc[1])),
            game_f->Render(),
        });
    });

    // Create a Screen with the Renderer.
    auto screen = ScreenInteractive::FitComponent();

    // Use Screen::Loop to continuously render and handle events.
    screen.Loop(component_renderer);

}