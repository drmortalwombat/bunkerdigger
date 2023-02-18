#ifndef TILES_H
#define TILES_H

#define TF_NONE		0x00
#define TF_LEFT		0x01
#define TF_RIGHT	0x02
#define TF_UP		0x04
#define TF_DOWN		0x08

#define TF_BUNKER	0x10
#define TF_ROOM		0x20

#define TF_LR		TF_LEFT | TF_RIGHT

extern const char TileFlags[];

extern char BunkerMapData[];


void tiles_draw(char sx, char sy);

void tile_dig(char x, char y);

#pragma compile("tiles.c")

#endif

