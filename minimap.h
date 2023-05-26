#ifndef MINIMAP_H
#define MINIMAP_H

extern char * const MinimapHiresTab[16];

void minimap_draw(void);

void minimap_highlight(char sx, char sy);

#pragma compile("minimap.c")

#endif