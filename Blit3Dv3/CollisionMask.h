#pragma once

/*
The bitshift operator '<<' makes it easy to make bitmasks
by shifting bit 1 to the left.

Filter masks in Box2D are uint16, so we can account for
16 different categories of objects.
*/


#define CMASK_BRICK 	(1 << 0)
#define CMASK_BALL 		(1 << 1)
#define CMASK_PADDLE 	(1 << 2)
#define CMASK_EDGES 	(1 << 3)
#define CMASK_GROUND 	(1 << 4)
#define CMASK_POWERUP	(1 << 5)
