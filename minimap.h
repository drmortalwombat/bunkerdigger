#ifndef MINIMAP_H
#define MINIMAP_H

extern __striped char * const MinimapHiresTab[16];

// Draw minimap
void minimap_draw(void);

// Highlight room in minimap
void minimap_highlight(char sx, char sy);

#pragma compile("minimap.c")

#endif