/*
	SuperCollider video server integration.
	Copyright (c) 2019 Rory Green.

	====================================================================

	SuperCollider real time audio synthesis system
	Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <string.h>           // for strlen
#include <stdint.h>           // for int32_t, uint32_t
#include <string>             // for string
#include <zmq.h>              // for zmq_send, zmq_strerror, ZMQ_DONTWAIT

#include "SC_Graph.h"         // for Graph
#include "SC_Unit.h"          // for Unit
#include "SC_UnitDef.h"       // for UnitDef
#include "SC_Wire.h"          // for Wire
#include "SC_World.h"         // for World
#include "SC_WorldOptions.h"  // for scprintf
#include "SC_Prototypes.h"    // for World_Alloc, World_Free
#include "json.hpp"           // for json_ref, basic_json<>::value_type, json
#include "SC_Node.h"          // for Node
#include "SC_CoreAudio.h"     // for SC_AudioDriver
#include "SC_HiddenWorld.h"   // for HiddenWorld
#include "SC_FifoMsg.h"       // for FifoMsg

using json = nlohmann::json;

typedef struct GLWindowNew {
	World *inWorld;
	int32_t id;
	int32_t width;
	int32_t height;
} GLWindowNew_t;

typedef struct GLWindowFree {
	World *inWorld;
	int32_t windowID;
} GLWindowFree_t;

typedef struct GLVideoNew {
	World *inWorld;
	int32_t id;
	const char *filepath;
	float rate;
	bool loop;
	int32_t windowID;
} GLVideoNew_t;

typedef struct GLVideoFree {
	World *inWorld;
	int32_t id;
	int32_t windowID;
} GLVideoFree_t;

typedef struct GLImageNew {
	World *inWorld;
	int32_t id;
	const char *filepath;
	int32_t windowID;
} GLImageNew_t;

typedef struct GLImageFree {
	World *inWorld;
	int32_t id;
	int32_t windowID;
} GLImageFree_t;

typedef struct GLDelBufNew {
	World *inWorld;
	int32_t id;
	int32_t len;
	int32_t windowID;
} GLDelBufNew_t;

typedef struct GLDelBufFree {
	World *inWorld;
	int32_t id;
	int32_t windowID;
} GLDelBufFree_t;

typedef struct {
	World *mWorld;
	int32_t nodeID;
} NodeFree_t;


void process_graph_fn(FifoMsg *inMsg);
void video_free_node_fn(FifoMsg *inMsg);

void create_gl_window_fn(FifoMsg *inMsg);
void create_gl_video_fn(FifoMsg *inMsg);
void create_gl_read_video_fn(FifoMsg *inMsg);
void create_gl_image_fn(FifoMsg *inMsg);
void create_gl_delBuf_fn(FifoMsg *inMsg);

void free_gl_window_fn(FifoMsg *inMsg);
void free_gl_video_fn(FifoMsg *inMsg);
void free_gl_image_fn(FifoMsg *inMsg);
void free_gl_delBuf_fn(FifoMsg *inMsg);


void process_graph(World *inWorld, Graph *graph)
{
	FifoMsg msg;
	msg.Set(inWorld, process_graph_fn, 0, (void *)graph);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}

void video_free_node(World *inWorld, int32_t nodeID)
{
	FifoMsg msg;
	NodeFree_t *data = (NodeFree_t *) World_Alloc(inWorld, sizeof(NodeFree_t));
	data->mWorld = inWorld;
	data->nodeID = nodeID;
	msg.Set(inWorld, video_free_node_fn, 0, (void *)data);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}


void create_gl_window(World *inWorld, int32_t windowID, int32_t width, int32_t height)
{
	FifoMsg msg;

	GLWindowNew_t *window = (GLWindowNew_t *) World_Alloc(inWorld, sizeof(GLWindowNew_t));
	window->inWorld = inWorld;
	window->id = windowID;
	window->width = width;
	window->height = height;

	msg.Set(inWorld, create_gl_window_fn, 0, (void *)window);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}

void create_gl_video(World *inWorld, int32_t videoID, const char *videoPath, int32_t windowID)
{
	FifoMsg msg;

	GLVideoNew_t *video = (GLVideoNew_t *) World_Alloc(inWorld, sizeof(GLVideoNew_t));
	video->inWorld = inWorld;
	video->id = videoID;
	video->filepath = videoPath;
	video->windowID = windowID;

	msg.Set(inWorld, create_gl_video_fn, 0, (void *)video);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}

void create_gl_read_video(World *inWorld, int32_t videoID, const char *videoPath, int32_t windowID)
{
	FifoMsg msg;

	GLVideoNew_t *video = (GLVideoNew_t *) World_Alloc(inWorld, sizeof(GLVideoNew_t));
	video->inWorld = inWorld;
	video->id = videoID;
	video->filepath = videoPath;
	video->windowID = windowID;

	msg.Set(inWorld, create_gl_read_video_fn, 0, (void *)video);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}

void create_gl_image(World *inWorld, int32_t imageID, const char *imagePath, int32_t windowID)
{
	FifoMsg msg;

	GLImageNew_t *image = (GLImageNew_t *) World_Alloc(inWorld, sizeof(GLImageNew_t));
	image->inWorld = inWorld;
	image->id = imageID;
	image->filepath = imagePath;
	image->windowID = windowID;

	msg.Set(inWorld, create_gl_image_fn, 0, (void *)image);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}

void create_gl_delBuf(World *inWorld, int32_t bufID, int32_t bufLen, int32_t windowID)
{
	FifoMsg msg;

	GLDelBufNew_t *delBuf = (GLDelBufNew_t *) World_Alloc(inWorld, sizeof(GLDelBufNew_t));
	delBuf->inWorld = inWorld;
	delBuf->id = bufID;
	delBuf->len = bufLen;
	delBuf->windowID = windowID;

	msg.Set(inWorld, create_gl_delBuf_fn, 0, (void *)delBuf);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}


void free_gl_window(World *inWorld, int32_t windowID)
{
	FifoMsg msg;

	GLWindowFree_t *window = (GLWindowFree_t *) World_Alloc(inWorld, sizeof(GLWindowFree_t));
	window->inWorld = inWorld;
	window->windowID = windowID;

	msg.Set(inWorld, free_gl_window_fn, 0, (void *)window);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}

void free_gl_video(World *inWorld, int32_t videoID, int32_t windowID)
{
	FifoMsg msg;

	GLVideoFree_t *video = (GLVideoFree_t *) World_Alloc(inWorld, sizeof(GLVideoFree_t));
	video->inWorld = inWorld;
	video->id = videoID;
	video->windowID = windowID;

	msg.Set(inWorld, free_gl_video_fn, 0, (void *)video);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}

void free_gl_image(World *inWorld, int32_t imageID, int32_t windowID)
{
	FifoMsg msg;

	GLImageFree_t *image = (GLImageFree_t *) World_Alloc(inWorld, sizeof(GLImageFree_t));
	image->inWorld = inWorld;
	image->id = imageID;
	image->windowID = windowID;

	msg.Set(inWorld, free_gl_image_fn, 0, (void *)image);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}

void free_gl_delBuf(World *inWorld, int32_t bufID, int32_t windowID)
{
	FifoMsg msg;

	GLDelBufFree_t *delBuf = (GLDelBufFree_t *) World_Alloc(inWorld, sizeof(GLDelBufFree_t));
	delBuf->inWorld = inWorld;
	delBuf->id = bufID;
	delBuf->windowID = windowID;

	msg.Set(inWorld, free_gl_delBuf_fn, 0, (void *)delBuf);
	inWorld->hw->mAudioDriver->SendMsgFromEngine(msg);
}



/* Functions to be executed in NRT thread */



void process_graph_fn(FifoMsg *inMsg)
{
	Graph *graph = (Graph *) inMsg->mData;
	// OSC_Packet* packet = (OSC_Packet *) World_Alloc(graph->mNode.mWorld, sizeof(OSC_Packet));

	json j = {
		{"tag", "GraphNew"},
		{"nodeID", graph->mNode.mID},
		{"units", json::array({})}
	};

	Unit *unit;
	Wire *wire;

	for (uint32_t i = 0; i < graph->mNumCalcUnits; i++) {
		unit = graph->mCalcUnits[i];

		json j_unit;
		j_unit["scUnitName"] = (char *) unit->mUnitDef->mUnitDefName;
		j_unit["scUnitID"] = unit->mUnitIndex;
		j_unit["scNodeID"] = graph->mNode.mID;
		j_unit["scUnitInputs"] = json::array({});
		j_unit["scUnitOutputs"] = json::array({});

		for (uint32_t j = 0; j < unit->mNumInputs; j++) {
			wire = unit->mInput[j];
			j_unit["scUnitInputs"][j] = wire->mWireID;
		}

		for (uint32_t j = 0; j < unit->mNumOutputs; j++) {
			wire = unit->mOutput[j];
			j_unit["scUnitOutputs"][j] = wire->mWireID;
		}

		j["units"][i] = j_unit;
	}

	void *sock = graph->mNode.mWorld->mCmdMsgSock;

	std::string msg = j.dump();

	scprintf("*** Debug: sending graph msg: %s\n", msg.c_str());

	int rc = zmq_send(sock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}
}

void video_free_node_fn(FifoMsg *inMsg)
{
	NodeFree_t *data = (NodeFree_t *) inMsg->mData;
	World *inWorld = data->mWorld;
	int32_t nodeID = data->nodeID;

	json j = {
		{"tag", "GraphFree"},
		{"nodeID", nodeID}
	};

	int bytes;

	std::string msg = j.dump();

	scprintf("*** Debug: sending graph msg: %s\n", msg.c_str());

	int rc = zmq_send(inWorld->mCmdMsgSock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}
}

void create_gl_window_fn(FifoMsg *inMsg)
{
	GLWindowNew_t *window = (GLWindowNew_t *) inMsg->mData;

	json j = {
		{"tag", "GLWindowNew"},
		{"windowID", window->id},
		{"windowWidth", window->width},
		{"windowHeight", window->height}
	};

	int bytes;
	std::string msg = j.dump();

	scprintf("*** Debug: sending GL msg: %s\n", msg.c_str());

	int rc = zmq_send(window->inWorld->mCmdMsgSock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}

	World_Free(window->inWorld, window);
}

void create_gl_video_fn(FifoMsg *inMsg)
{
	GLVideoNew_t *video = (GLVideoNew_t *) inMsg->mData;

	json j = {
		{"tag", "GLVideoNew"},
		{"videoID", video->id},
		{"videoPath", video->filepath},
		// {"videoRate", video->rate},
		// {"videoLoop", video->loop},
		{"windowID", video->windowID}
	};

	int bytes;
	std::string msg = j.dump();

	scprintf("*** Debug: sending GL msg: %s\n", msg.c_str());

	int rc = zmq_send(video->inWorld->mCmdMsgSock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}

	World_Free(video->inWorld, video);
}

void create_gl_read_video_fn(FifoMsg *inMsg)
{
	GLVideoNew_t *video = (GLVideoNew_t *) inMsg->mData;

	json j = {
		{"tag", "GLVideoRead"},
		{"videoID", video->id},
		{"videoPath", video->filepath},
		// {"videoRate", video->rate},
		// {"videoLoop", video->loop},
		{"windowID", video->windowID}
	};

	int bytes;
	std::string msg = j.dump();

	scprintf("*** Debug: sending GL msg: %s\n", msg.c_str());

	int rc = zmq_send(video->inWorld->mCmdMsgSock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}

	World_Free(video->inWorld, video);
}

void create_gl_image_fn(FifoMsg *inMsg)
{
	GLImageNew_t *image = (GLImageNew_t *) inMsg->mData;

	json j = {
		{"tag", "GLImageNew"},
		{"imageID", image->id},
		{"imagePath", image->filepath},
		{"windowID", image->windowID}
	};

	int bytes;
	std::string msg = j.dump();

	scprintf("*** Debug: sending GL msg: %s\n", msg.c_str());

	int rc = zmq_send(image->inWorld->mCmdMsgSock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}

	World_Free(image->inWorld, image);
}

void create_gl_delBuf_fn(FifoMsg *inMsg)
{
	GLDelBufNew_t *delBuf = (GLDelBufNew_t *) inMsg->mData;

	json j = {
		{"tag", "DelBufNew"},
		{"bufferID", delBuf->id},
		{"bufferLen", delBuf->len},
		{"windowID", delBuf->windowID}
	};

	int bytes;
	std::string msg = j.dump();

	scprintf("*** Debug: sending GL msg: %s\n", msg.c_str());

	int rc = zmq_send(delBuf->inWorld->mCmdMsgSock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}

	World_Free(delBuf->inWorld, delBuf);
}

void free_gl_window_fn(FifoMsg *inMsg)
{
	GLWindowFree_t *window = (GLWindowFree_t *) inMsg->mData;

	json j = {
		{"tag", "GLWindowFree"},
		{"windowID", window->windowID}
	};

	int bytes;
	std::string msg = j.dump();

	scprintf("*** Debug: sending GL msg: %s\n", msg.c_str());

	int rc = zmq_send(window->inWorld->mCmdMsgSock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}

	World_Free(window->inWorld, window);
}

void free_gl_video_fn(FifoMsg *inMsg)
{
	GLVideoFree_t *video = (GLVideoFree_t *) inMsg->mData;

	json j = {
		{"tag", "GLVideoFree"},
		{"videoID", video->id},
		{"windowID", video->windowID}
	};

	int bytes;
	std::string msg = j.dump();

	scprintf("*** Debug: sending GL msg: %s\n", msg.c_str());

	int rc = zmq_send(video->inWorld->mCmdMsgSock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}

	World_Free(video->inWorld, video);
}

void free_gl_image_fn(FifoMsg *inMsg)
{
	GLImageFree_t *image = (GLImageFree_t *) inMsg->mData;

	json j = {
		{"tag", "GLImageFree"},
		{"imageID", image->id},
		{"windowID", image->windowID}
	};

	int bytes;
	std::string msg = j.dump();

	scprintf("*** Debug: sending GL msg: %s\n", msg.c_str());

	int rc = zmq_send(image->inWorld->mCmdMsgSock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}

	World_Free(image->inWorld, image);
}

void free_gl_delBuf_fn(FifoMsg *inMsg)
{
	GLDelBufFree_t *delBuf = (GLDelBufFree_t *) inMsg->mData;

	json j = {
		{"tag", "DelBufFree"},
		{"bufferID", delBuf->id},
		{"windowID", delBuf->windowID}
	};

	int bytes;
	std::string msg = j.dump();

	scprintf("*** Debug: sending GL msg: %s\n", msg.c_str());

	int rc = zmq_send(delBuf->inWorld->mCmdMsgSock, msg.c_str(), strlen(msg.c_str()), ZMQ_DONTWAIT);
	if (rc < 0) {
		scprintf("*** Error: could not send message to video server. zmq_send error: %s\n", zmq_strerror(errno));
	}

	World_Free(delBuf->inWorld, delBuf);
}
