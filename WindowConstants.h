#ifndef WINDOWCONSTANTS_H_
#define WINDOWCONSTANTS_H_

/*
	Game Window
      ______________________________________
	 |  Header                              |                         
	 |___________ __________________________|
	 |           |                          |
	 |  Side     |    Game                  |
	 |           |                          |
	 |           |                          |
	 |           |                          |
	 |           |                          |
	 |           |  _                       |
	 |           | |_| <- Tile              |
	 |___________|__________________________|

*/

// (0, 0) position is top-left corner

// Units: pixels

// W stands for width, 
// H - height, 
// PAD - padding, HPAD - height padding, WPAD - width padding

// Margin and padding are used here interchangeably

// Size is used when W == H

const int WINDOW_H = 640, WINDOW_W = 740,
	      WINDOW_WPAD = 24, WINDOW_HPAD = 14,
		  HEADER_W = 692, HEADER_H = 80, 
		  SIDE_W = 160, SIDE_H = 532, 
		  GAME_SIZE = 532, GAME_PAD = 12, 
		  TILE_PAD = 4, TILE_SIZE = 60;

const int GAME_X = WINDOW_WPAD + SIDE_W + GAME_PAD, // Origin of game zone relative to window's coordinate system 
          GAME_Y = WINDOW_HPAD + HEADER_H + GAME_PAD;



#endif // WINDOWCONSTANTS_H_
