#include <stdint.h>    // for int32_t

#include "SC_Graph.h"  // for Graph

void process_graph(World *inWorld, Graph *graph);
void video_free_node(World *inWorld, int32_t nodeID);
void create_gl_window(World *inWorld, int32_t windowID, int32_t width, int32_t height);
void create_gl_video(World *inWorld, int32_t videoID, const char *videoPath, float rate, bool loop, int32_t windowID);
void create_gl_read_video(World *inWorld, int32_t videoID, const char *videoPath, float rate, bool loop, int32_t windowID);
void create_gl_image(World *inWorld, int32_t imageID, const char *imagePath, int32_t windowID);
void free_gl_window(World *inWorld, int32_t windowID);
void free_gl_video(World *inWorld, int32_t videoID, int32_t windowID);
void free_gl_image(World *inWorld, int32_t imageID, int32_t windowID);
