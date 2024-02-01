/**
 * this is a project i started to learn c++ and the FTXUI library
*/
#include <string>   
#include <utility> 
#include <vector>   
#include <random>

#include <ftxui/dom/elements.hpp> 
#include <ftxui/screen/screen.hpp>  
#include "ftxui/component/captured_mouse.hpp" 
#include "ftxui/component/component.hpp"  
#include "ftxui/component/component_base.hpp" 
#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/component/loop.hpp"       // for Loop
#include "ftxui/screen/color.hpp" 

#include "shapes.h"



using namespace ftxui;

const int GAME_WIDTH = 10;
const int GAME_HEIGHT = 20;
int GAME_BLOCK_SIZE = 8;

Color colors[7] = {Color::Purple, Color::Yellow1, Color::SkyBlue1, Color::Red, Color::LightGreen, Color::DarkOrange, Color::DarkBlue};

class CoolCanvas : public Canvas {
    public:
        CoolCanvas(int x, int y) : Canvas(x, y)
        {

        }
        void drawGrid(int8_t sizex, int8_t sizey, int8_t x, int8_t y) {
            int xinc = sizex/x;
            for (int xAxis = 0; xAxis <= x; xAxis++) {
                DrawPointLine(xinc*xAxis, 0, xinc*xAxis, 200, Color::Gold1);
            }
            for (int yAxis = 0; yAxis <= y; yAxis++) {
                DrawPointLine(0, yAxis*10, 100, yAxis*10, Color::Red);
            }
        }

        int emptyGrids(int game[][GAME_WIDTH]) {
                int amm = 0;
                int n = sizeof(game[0]) / sizeof(game[0][0]);

                for (int y = GAME_HEIGHT; y > 0; y--) {

                    if (std::none_of(game[y], game[y]+n, [](int i) {
                        return i == 0;
                    })) {
                        for (int a = y-1; a > 0; a--) {
                            std::copy(std::begin(game[a]), std::end(game[a]), std::begin(game[a+1]));
                        }
                        y = GAME_HEIGHT;
                        amm++;
                    }
                }
                
                return amm;
        }

        void filledBlock(int x, int y, int size, Color color) {
            for (int loop = 0; loop < size; loop++)
                DrawBlockLine(x+loop, y, x+loop, y+size, color);
        }

        void outlineBlock(int x, int y, int size, Color color) {
            for (int loop = 0; loop < size; ++loop)
                DrawPointLine(x+loop, y, x+loop, y+size, color);
            // DrawPointLine(x, y, x+size, y, color);
            // DrawPointLine(x, y, x, y+size, color);
            // DrawPointLine(x+size, y, x+size, y+size, color);
            // DrawPointLine(x, y+size, x+size, y+size, color);
        }


        void drawBlocks() {
            for (int y = 0; y < GAME_HEIGHT; y++) {
                for (int x = 0; x < GAME_WIDTH; x++) 
                    filledBlock(x*10+5, y*10+5, 1, Color::GrayDark);
            }
        }

        void drawGameBlocks(int arr[][GAME_WIDTH]) {
            for (int y = 0; y < GAME_HEIGHT; y++) {
                for (int x = 0; x < GAME_WIDTH; x++) {
                    if (arr[y][x] != 0)
                        filledBlock(x*10, y*10, GAME_BLOCK_SIZE, colors[arr[y][x]-1]);
                }
            }
        }

        void renderPlayerPiece(struct Shape block, int origin[2]) {
            int piece_x = origin[0]*10;
            int piece_y = origin[1]*10;
            for (int x = 0; x < 4; x++) {
                for(int y = 0; y < 4; y++) {
                    if (block.shape[block.rot%4][y][x] != 0) {
                        filledBlock(piece_x+x*10, piece_y+y*10, GAME_BLOCK_SIZE, colors[block.id-1]);
                    }
                }
            }
        }

        void renderOutline(struct Shape block, int origin[2]) {
            int piece_x = origin[0]*10;
            int piece_y = origin[1]*10;
            for (int x = 0; x < 4; x++) {
                for(int y = 0; y < 4; y++) {
                    if (block.shape[block.rot%4][y][x] != 0) {
                        outlineBlock(piece_x+x*10, piece_y+y*10, GAME_BLOCK_SIZE, colors[block.id-1]);
                    }
                }
            }
        }

        void drawHeld(struct Shape block, int xoff, int yoff) {
            for (int y = 0; y < 4; ++y) {
                for (int x = 0; x < 4; ++x)
                    if (block.shape[block.rot%4][y][x] != 0)
                        filledBlock(x*4 + block.offset[0] + xoff, y*4 + block.offset[1] + yoff, 2, colors[block.id-1]);
            }
        }

        void drawUpcomming(std::vector<struct Shape> shapeList) {
            for (int i = 0; i < shapeList.size(); ++i)
                drawHeld(shapeList.at(i), 0, i*15);
        }


};

struct Shape randomShape() {
    return SHAPES[rand() % 7];
}

bool pieceHasRoom(struct Shape block, int atOrigin[2], int game[][GAME_WIDTH]) {
    int start_x = atOrigin[0];
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (block.shape[block.rot%4][y][x] == 1 && (start_x+x >= GAME_WIDTH || start_x+x < 0)) 
                return false;
            if (atOrigin[1]+y < 0) {
                continue;
            }
            if (block.shape[block.rot%4][y][x] == 1 && (game[atOrigin[1]+y][x+start_x] != 0 || atOrigin[1]+y >= GAME_HEIGHT) )
                return false;
        }
    }
    return true;
}

bool dropBlock(struct Shape block, int spot[2], int game[][GAME_WIDTH]) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (block.shape[block.rot%4][y][x] == 1) {
                if (y+spot[1] < 0) return false;
                game[y+spot[1]][x+spot[0]] = block.id;
            }
        }
    }

    return true;
}

enum Controls {left, right, softDrop, hardDrop, clockwiseRotate, counterClockwiseRotate, fullRotate, hold};

const Event controlsArray[][3] = {
	{Event::Special("\x1B[D")}, // Move piece left (left arrow is awful oml)
	{Event::Special("\x1B[C")}, // Move piece right
	{Event::Special("\x1B[B")}, // Soft drop
	{Event::Character(' ')}, // Hard drop
	{Event::Special("\x1B[A"), Event::Character('x')}, // Rotate
	{Event::Character('z')}, // Rotate the other way
	{Event::Character('a')}, // Rotate 180 deg
	{Event::Character('c')} // Hold
};

int main() {
    srand(time(NULL)); // set random seed
    std::vector<struct Shape> shapes = {};
    for (int i = 0; i <= 5; ++i) shapes.push_back(randomShape());

    int scoreValues[5] = {0, 40, 100, 300, 1200};
    int pieceLoc[2] = {5, -3};
    int displayPos[2] = {0, 0};
    int gameArray[GAME_HEIGHT][GAME_WIDTH] = {0};
    int frames = 0;
    int score = 0;
    int menu = 0;
    int gameSpeed = 25;

    struct Shape currentShape = randomShape();
    struct Shape heldBlock;
    bool hasHeld = 0;

    auto screen = ScreenInteractive::FitComponent();
    auto c = CoolCanvas(GAME_WIDTH * 10, GAME_HEIGHT * 10);
    auto blockDisplay = CoolCanvas(14, 12);
    auto blockListDisplay = CoolCanvas(14, 75);

    std::vector<std::string> menu_entries = {"Tetris", "Credits"};

    auto radiobox = Menu(&menu_entries, &menu, MenuOption::Horizontal());
    auto credits = vbox({
        text("that sum' for loop - titty") | bold | center,
        text("holding the L - @knob") | bold | center,
        text("literally everything else - me") | bold | color(Color::Gold1) | center
    }) | size(WIDTH, EQUAL, 50);

    auto held_display = Renderer([&] {
        if (heldBlock.id != -1) blockDisplay.drawHeld(heldBlock, 0, 0);
        return canvas(std::move(blockDisplay));
    });

    auto blockList = Renderer([&] {
        blockListDisplay.drawUpcomming(shapes);
        return canvas(std::move(blockListDisplay));
    });

    auto game_area = Renderer(radiobox, [&] {

        switch (menu){
            case 1:
                return credits;
        }
        
        if (!pieceHasRoom(currentShape, pieceLoc, gameArray)) {
                pieceLoc[1]--;
                hasHeld = false;
                // check if game should be over;
                bool dropped = dropBlock(currentShape, pieceLoc, gameArray);
                if (!dropped) {
                    std::fill(&gameArray[0][0], &gameArray[0][0] + sizeof(gameArray)/sizeof(int), 0);
                    heldBlock = {};
                    score = 0;
                }

                score += scoreValues[c.emptyGrids(gameArray)];
                
                pieceLoc[0] = 5; pieceLoc[1] = -3;

                currentShape = shapes.at(0);
                shapes.push_back(randomShape());
                shapes.erase(shapes.begin());
                
                frames = 0;
                
            }

        if (frames%gameSpeed == 0)
            pieceLoc[1]++;

        
        c.drawBlocks();
        c.drawGameBlocks(gameArray);
        
        c.renderOutline(currentShape, displayPos);
        c.renderPlayerPiece(currentShape, pieceLoc);

        screen.PostEvent(Event::Custom); // This will force the canvas to update every frame.
        return canvas(std::move(c));

    }) | CatchEvent([&](Event event) {
        if (menu != 0) return false;
        if (event.is_mouse()) return false;
        // outline of where block will go
        // only needs updated on input
        displayPos[0] = pieceLoc[0]; displayPos[1] = -1;
        while(pieceHasRoom(currentShape, displayPos, gameArray)) displayPos[1]++;
        displayPos[1]--;
		
		for (int i = 0; i < 8; ++i) {
			if (std::find(std::begin(controlsArray[i]), std::end(controlsArray[i]), event) != std::end(controlsArray[i])) {
				switch (i) {
					case Controls::left:
						pieceLoc[0]--;
						if (!pieceHasRoom(currentShape, pieceLoc, gameArray))
							pieceLoc[0]++;
						return true;
					case Controls::right:
						pieceLoc[0]++;
						if (!pieceHasRoom(currentShape, pieceLoc, gameArray))
							pieceLoc[0]--;
						return true;
					case Controls::softDrop:
						frames=gameSpeed;
						return true;
					case Controls::clockwiseRotate:
						currentShape.rot++;
            			if (!pieceHasRoom(currentShape, pieceLoc, gameArray))
            				currentShape.rot--;
						return true;
					case Controls::counterClockwiseRotate:
						currentShape.rot--;
            			if (!pieceHasRoom(currentShape, pieceLoc, gameArray))
            				currentShape.rot++;
						return true;
					case Controls::fullRotate:
						currentShape.rot -= 2;
            			if (!pieceHasRoom(currentShape, pieceLoc, gameArray))
            				currentShape.rot += 2;
						return true;
					case Controls::hardDrop:
						while (pieceHasRoom(currentShape, pieceLoc, gameArray))
                			pieceLoc[1]++;
            			pieceLoc[1]--;
            			frames=gameSpeed;
						return true;
					case Controls::hold:
						if (hasHeld) return true;

						pieceLoc[0] = 5; pieceLoc[1] = -3;
						if (heldBlock.id == -1) {
							heldBlock = shapes.at(0);
							shapes.erase(shapes.begin());
							shapes.push_back(randomShape());
						}

						struct Shape temp = heldBlock;
						heldBlock = currentShape;
						heldBlock.rot = 0;
						currentShape = temp;
						hasHeld = true;

						return true;
				}
			}
		}
		
		return false;
    });    
    
    auto component_renderer = Renderer(game_area, [&] {
        return vbox({
            separatorLight() | size(WIDTH, EQUAL, 48),
            text("Score: " + std::to_string(score)) | center | size(WIDTH, EQUAL, 48),
            radiobox->Render()  | center | size(WIDTH, EQUAL, 48),
            separatorLight() | flex | size(WIDTH, EQUAL, 48),
            hbox({
                game_area->Render() | border | flex,
                vbox({
                    held_display->Render() | border,
                    blockList->Render() | border
                    }) | flex
                })
        });
    });

  Loop loop(&screen, component_renderer);
 
  while (!loop.HasQuitted()) {
    frames++;
    loop.RunOnce();
    std::this_thread::sleep_for(std::chrono::milliseconds(20)); // ~50 fps
  }

  return EXIT_SUCCESS;

}
