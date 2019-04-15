#pragma once
#ifdef __cplusplus
extern "C" {
#endif

enum { ATLAS_WIDTH = 128, ATLAS_HEIGHT = 128 };
extern const unsigned char atlas_texture[ATLAS_HEIGHT][ATLAS_WIDTH];
extern const mu_Rect atlas[];

#ifdef __cplusplus
}
#endif

