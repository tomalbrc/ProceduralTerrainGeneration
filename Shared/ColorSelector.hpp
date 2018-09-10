/**
 * Tom Albrecht
 * 23.08.18
 */

#include "bitmap_image.h"

class ColorSelector {
public:
	static rgb_t ColorFromValue(double value) {
		rgb_t col;
		col.red = 0;
		col.green = 100;
		col.blue = 255;

		value *= 100.0;

        
        if (value > 1) {
            col.red = 230;
            col.green = 240;
            col.blue = 70;
        }
		if (value > 4) {
			col.red = 70;
			col.green = 240;
			col.blue = 20;
		}
        if (value > 20) {
            col.red = 130;
            col.green = 16;
            col.blue = 20;
        }
        if (value > 23) {
			col.red = 80;
			col.green = 40;
			col.blue = 10;
		}

		return col;
	}
};
