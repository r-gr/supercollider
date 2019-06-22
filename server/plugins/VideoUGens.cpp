/*
	SuperCollider video UGens.
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

#include <stdio.h>              // for printf
#include <bits/stdint-intn.h>   // for int32_t
#include <stdint.h>             // for uint_fast8_t
#include <zmq.h>                // for zmq_send

#include "SC_PlugIn.h"

// To avoid unnecessarily high IPC message sending rates, divide the control rate
// by this factor i.e. send a message every nth control rate sample.
#define CONTROL_RATE_DIVISOR 8
// The rateDivideCounter has a uint type (> 8 bits) so will wrap around at the
// maxium value. This behaves particularly nicely for a control rate divisor
// which is a power of 2 because the maximum value of the uint will be a
// multiple of the divisor so the rate is divided evenly when wraparound occurs.
#define SHOULD_SEND (unit->rateDivideCounter++ % CONTROL_RATE_DIVISOR) == 0

struct IPCMsg
{
	uint16_t msgType;   // 0 = data; 1 = video; 2 = image
	uint16_t index;
	int32_t nodeID;
	int32_t unitID;
};

struct DataMsg : public IPCMsg
{
	float value;
};

struct VideoMsg : public IPCMsg
{
	int32_t videoID;
};

struct ImageMsg : public IPCMsg
{
	int32_t imageID;
};

//////////////////////////////////////////////////////////////////////////////////

static InterfaceTable *ft;

struct VideoUnit : public Unit
{
	// C99 'fast' data type will use the highest performance width integer (greater
	// than or equal to 8 bits wide in this case) for the target platform.
	uint_fast8_t rateDivideCounter;
};

struct GLRed : public VideoUnit
{
	DataMsg  *msgRed;
};

struct GLGreen : public VideoUnit
{
	DataMsg  *msgGreen;
};

struct GLBlue : public VideoUnit
{
	DataMsg  *msgBlue;
};

struct GLAlpha : public VideoUnit
{
	DataMsg  *msgAlpha;
};

struct GLOpacity : public VideoUnit
{
	DataMsg  *msgOpacity;
};

struct GLWhite : public VideoUnit
{
};

struct GLOut : public VideoUnit
{
};

struct GLPlayImg : public VideoUnit
{
	ImageMsg *msgImgID;
};

struct GLPlayVid : public VideoUnit
{
	VideoMsg *msgVidID;
};

struct GLPrevFrame : public VideoUnit
{
};

struct GLPrevFrame2 : public VideoUnit
{
};

struct GLRGB : public VideoUnit
{
	DataMsg  *msgR, *msgG, *msgB;
};

struct GLRGBA : public VideoUnit
{
	DataMsg  *msgR, *msgG, *msgB, *msgA;
};

struct GLMix : public VideoUnit
{
	DataMsg  *msgMix;
};

struct GLBlend : public VideoUnit
{
	DataMsg *msgBlendMode, *msgMix;
};

struct GLShowImgTex : public VideoUnit
{
	ImageMsg *msgImgID;
};

struct GLShowVidTex : public VideoUnit
{
	VideoMsg *msgVidID;
};

struct GLTexCoords : public VideoUnit
{
};

struct GLTexFlipX : public VideoUnit
{
};

struct GLTexFlipY : public VideoUnit
{
};

struct GLTexMirrorX : public VideoUnit
{
	DataMsg  *msgMirrorX, *msgPolarity;
};

struct GLTexMirrorY : public VideoUnit
{
	DataMsg  *msgMirrorY, *msgPolarity;
};

struct GLTexScale : public VideoUnit
{
	DataMsg  *msgScaleX, *msgScaleY;
};

struct GLTexTrans : public VideoUnit
{
	DataMsg  *msgTransX, *msgTransY;
};

struct GLRotate : public VideoUnit
{
	DataMsg  *msgAngle;
};

//////////////////////////////////////////////////////////////////////////////////

struct SHKCheckerboard : public VideoUnit
{
	DataMsg *msgRows, *msgCols;
};

struct SHKCircleWave : public VideoUnit
{
	DataMsg *msgTime, *msgSpeed, *msgBrightness, *msgStrength, *msgDensity, *msgCenter;
};

struct SHKColorInvert : public VideoUnit
{
};

struct SHKDesaturate : public VideoUnit
{
	DataMsg *msgStrength;
};

struct SHKWater : public VideoUnit
{
	VideoMsg *msgVidID;
	DataMsg  *msgTime, *msgSpeed, *msgStrength, *msgFreq;
};

//////////////////////////////////////////////////////////////////////////////////

struct GLFunc1 : public VideoUnit
{
	DataMsg  *msg1;
};

struct GLFunc2 : public VideoUnit
{
	DataMsg  *msg1, *msg2;
};

struct GLFunc3 : public VideoUnit
{
	DataMsg  *msg1, *msg2, *msg3;
};

struct GLFunc4 : public VideoUnit
{
	DataMsg  *msg1, *msg2, *msg3, *msg4;
};

struct GLFunc5 : public VideoUnit
{
	DataMsg  *msg1, *msg2, *msg3, *msg4, *msg5;
};

struct GLFunc6 : public VideoUnit
{
	DataMsg  *msg1, *msg2, *msg3, *msg4, *msg5, *msg6;
};

struct GLFunc7 : public VideoUnit
{
	DataMsg  *msg1, *msg2, *msg3, *msg4, *msg5, *msg6, *msg7;
};

struct GLFunc8 : public VideoUnit
{
	DataMsg  *msg1, *msg2, *msg3, *msg4, *msg5, *msg6, *msg7, *msg8;
};

struct GLFunc9 : public VideoUnit
{
	DataMsg  *msg1, *msg2, *msg3, *msg4, *msg5, *msg6, *msg7, *msg8, *msg9;
};

struct GLFunc10 : public VideoUnit
{
	DataMsg  *msg1, *msg2, *msg3, *msg4, *msg5, *msg6, *msg7, *msg8, *msg9, *msg10;
};

//////////////////////////////////////////////////////////////////////////////////

extern "C"
{

void GLRed_Ctor(GLRed *unit);
void GLRed_next_k(GLRed *unit, int inNumSamples);
void GLRed_Dtor(GLRed *unit);

void GLGreen_Ctor(GLGreen *unit);
void GLGreen_next_k(GLGreen *unit, int inNumSamples);
void GLGreen_Dtor(GLGreen *unit);

void GLBlue_Ctor(GLBlue *unit);
void GLBlue_next_k(GLBlue *unit, int inNumSamples);
void GLBlue_Dtor(GLBlue *unit);

void GLAlpha_Ctor(GLAlpha *unit);
void GLAlpha_next_k(GLAlpha *unit, int inNumSamples);
void GLAlpha_Dtor(GLAlpha *unit);

void GLOpacity_Ctor(GLOpacity *unit);
void GLOpacity_next_k(GLOpacity *unit, int inNumSamples);
void GLOpacity_Dtor(GLOpacity *unit);

void GLWhite_Ctor(GLWhite *unit);
void GLWhite_next_k(GLWhite *unit, int inNumSamples);

void GLOut_Ctor(GLOut *unit);
void GLOut_next_k(GLOut *unit, int inNumSamples);

void GLPlayImg_Ctor(GLPlayImg *unit);
void GLPlayImg_next_k(GLPlayImg *unit, int inNumSamples);
void GLPlayImg_Dtor(GLPlayImg *unit);

void GLPlayVid_Ctor(GLPlayVid *unit);
void GLPlayVid_next_k(GLPlayVid *unit, int inNumSamples);
void GLPlayVid_Dtor(GLPlayVid *unit);

void GLPrevFrame_Ctor(GLPrevFrame *unit);
void GLPrevFrame_next_k(GLPrevFrame *unit, int inNumSamples);

void GLPrevFrame2_Ctor(GLPrevFrame2 *unit);
void GLPrevFrame2_next_k(GLPrevFrame2 *unit, int inNumSamples);

void GLRGB_Ctor(GLRGB *unit);
void GLRGB_next_k(GLRGB *unit, int inNumSamples);
void GLRGB_Dtor(GLRGB *unit);

void GLRGBA_Ctor(GLRGBA *unit);
void GLRGBA_next_k(GLRGBA *unit, int inNumSamples);
void GLRGBA_Dtor(GLRGBA *unit);

void GLMix_Ctor(GLMix *unit);
void GLMix_next_k(GLMix *unit, int inNumSamples);
void GLMix_Dtor(GLMix *unit);

void GLBlend_Ctor(GLBlend *unit);
void GLBlend_next_k(GLBlend *unit, int inNumSamples);
void GLBlend_Dtor(GLBlend *unit);

void GLShowImgTex_Ctor(GLShowImgTex *unit);
void GLShowImgTex_next_k(GLShowImgTex *unit, int inNumSamples);
void GLShowImgTex_Dtor(GLShowImgTex *unit);

void GLShowVidTex_Ctor(GLShowVidTex *unit);
void GLShowVidTex_next_k(GLShowVidTex *unit, int inNumSamples);
void GLShowVidTex_Dtor(GLShowVidTex *unit);

void GLTexCoords_Ctor(GLTexCoords *unit);
void GLTexCoords_next_k(GLTexCoords *unit, int inNumSamples);

void GLTexFlipX_Ctor(GLTexFlipX *unit);
void GLTexFlipX_next_k(GLTexFlipX *unit, int inNumSamples);

void GLTexFlipY_Ctor(GLTexFlipY *unit);
void GLTexFlipY_next_k(GLTexFlipY *unit, int inNumSamples);

void GLTexMirrorX_Ctor(GLTexMirrorX *unit);
void GLTexMirrorX_next_k(GLTexMirrorX *unit, int inNumSamples);
void GLTexMirrorX_Dtor(GLTexMirrorX *unit);

void GLTexMirrorY_Ctor(GLTexMirrorY *unit);
void GLTexMirrorY_next_k(GLTexMirrorY *unit, int inNumSamples);
void GLTexMirrorY_Dtor(GLTexMirrorY *unit);

void GLTexScale_Ctor(GLTexScale *unit);
void GLTexScale_next_k(GLTexScale *unit, int inNumSamples);
void GLTexScale_Dtor(GLTexScale *unit);

void GLTexTrans_Ctor(GLTexTrans *unit);
void GLTexTrans_next_k(GLTexTrans *unit, int inNumSamples);
void GLTexTrans_Dtor(GLTexTrans *unit);

void GLRotate_Ctor(GLRotate *unit);
void GLRotate_next_k(GLRotate *unit, int inNumSamples);
void GLRotate_Dtor(GLRotate *unit);


void SHKCheckerboard_Ctor(SHKCheckerboard *unit);
void SHKCheckerboard_next_k(SHKCheckerboard *unit, int inNumSamples);
void SHKCheckerboard_Dtor(SHKCheckerboard *unit);

void SHKCircleWave_Ctor(SHKCircleWave *unit);
void SHKCircleWave_next_k(SHKCircleWave *unit, int inNumSamples);
void SHKCircleWave_Dtor(SHKCircleWave *unit);

void SHKColorInvert_Ctor(SHKColorInvert *unit);
void SHKColorInvert_next_k(SHKColorInvert *unit, int inNumSamples);

void SHKDesaturate_Ctor(SHKDesaturate *unit);
void SHKDesaturate_next_k(SHKDesaturate *unit, int inNumSamples);
void SHKDesaturate_Dtor(SHKDesaturate *unit);

void SHKWater_Ctor(SHKWater *unit);
void SHKWater_next_k(SHKWater *unit, int inNumSamples);
void SHKWater_Dtor(SHKWater *unit);



void GLFunc1_Ctor(GLFunc1 *unit);
void GLFunc1_next_k(GLFunc1 *unit, int inNumSamples);
void GLFunc1_Dtor(GLFunc1 *unit);

void GLFunc2_Ctor(GLFunc2 *unit);
void GLFunc2_next_k(GLFunc2 *unit, int inNumSamples);
void GLFunc2_Dtor(GLFunc2 *unit);

void GLFunc3_Ctor(GLFunc3 *unit);
void GLFunc3_next_k(GLFunc3 *unit, int inNumSamples);
void GLFunc3_Dtor(GLFunc3 *unit);

void GLFunc4_Ctor(GLFunc4 *unit);
void GLFunc4_next_k(GLFunc4 *unit, int inNumSamples);
void GLFunc4_Dtor(GLFunc4 *unit);

void GLFunc5_Ctor(GLFunc5 *unit);
void GLFunc5_next_k(GLFunc5 *unit, int inNumSamples);
void GLFunc5_Dtor(GLFunc5 *unit);

void GLFunc6_Ctor(GLFunc6 *unit);
void GLFunc6_next_k(GLFunc6 *unit, int inNumSamples);
void GLFunc6_Dtor(GLFunc6 *unit);

void GLFunc7_Ctor(GLFunc7 *unit);
void GLFunc7_next_k(GLFunc7 *unit, int inNumSamples);
void GLFunc7_Dtor(GLFunc7 *unit);

void GLFunc8_Ctor(GLFunc8 *unit);
void GLFunc8_next_k(GLFunc8 *unit, int inNumSamples);
void GLFunc8_Dtor(GLFunc8 *unit);

void GLFunc9_Ctor(GLFunc9 *unit);
void GLFunc9_next_k(GLFunc9 *unit, int inNumSamples);
void GLFunc9_Dtor(GLFunc9 *unit);

void GLFunc10_Ctor(GLFunc10 *unit);
void GLFunc10_next_k(GLFunc10 *unit, int inNumSamples);
void GLFunc10_Dtor(GLFunc10 *unit);

}

//////////////////////////////////////////////////////////////////////////////////

void GLRed_Ctor(GLRed *unit)
{
	SETCALC(GLRed_next_k);
	unit->rateDivideCounter = 0;

	unit->msgRed = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgRed->msgType = 0;
	GLRed_next_k(unit, 1);
}

void GLRed_next_k(GLRed *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		float inRed = ZIN0(1); // get first sample

		// prepare messages to send to the video server
		unit->msgRed->nodeID = unit->mParent->mNode.mID;
		unit->msgRed->unitID = unit->mUnitIndex;
		unit->msgRed->index  = 1;
		unit->msgRed->value  = inRed;

		// send values to video server via nanomsg
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgRed, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLRed_Dtor(GLRed *unit)
{
	RTFree(unit->mWorld, unit->msgRed);
}

//////////////////////////////////////////////////////////////////////////////////

void GLGreen_Ctor(GLGreen *unit)
{
	SETCALC(GLGreen_next_k);
	unit->rateDivideCounter = 0;

	unit->msgGreen = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgGreen->msgType = 0;
	GLGreen_next_k(unit, 1);
}

void GLGreen_next_k(GLGreen *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		float inGreen = ZIN0(1); // get first sample

		// prepare messages to send to the video server
		unit->msgGreen->nodeID = unit->mParent->mNode.mID;
		unit->msgGreen->unitID = unit->mUnitIndex;
		unit->msgGreen->index  = 1;
		unit->msgGreen->value  = inGreen;

		// send values to video server via nanomsg
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgGreen, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLGreen_Dtor(GLGreen *unit)
{
	RTFree(unit->mWorld, unit->msgGreen);
}

//////////////////////////////////////////////////////////////////////////////////

void GLBlue_Ctor(GLBlue *unit)
{
	SETCALC(GLBlue_next_k);
	unit->rateDivideCounter = 0;

	unit->msgBlue = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgBlue->msgType = 0;
	GLBlue_next_k(unit, 1);
}

void GLBlue_next_k(GLBlue *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		float inBlue = ZIN0(1); // get first sample

		// prepare messages to send to the video server
		unit->msgBlue->nodeID = unit->mParent->mNode.mID;
		unit->msgBlue->unitID = unit->mUnitIndex;
		unit->msgBlue->index  = 1;
		unit->msgBlue->value  = inBlue;

		// send values to video server via nanomsg
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgBlue, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLBlue_Dtor(GLBlue *unit)
{
	RTFree(unit->mWorld, unit->msgBlue);
}

//////////////////////////////////////////////////////////////////////////////////

void GLAlpha_Ctor(GLAlpha *unit)
{
	SETCALC(GLAlpha_next_k);
	unit->rateDivideCounter = 0;

	unit->msgAlpha = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgAlpha->msgType = 0;
	GLAlpha_next_k(unit, 1);
}

void GLAlpha_next_k(GLAlpha *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		float inAlpha = ZIN0(1); // get first sample

		// prepare messages to send to the video server
		unit->msgAlpha->nodeID = unit->mParent->mNode.mID;
		unit->msgAlpha->unitID = unit->mUnitIndex;
		unit->msgAlpha->index  = 1;
		unit->msgAlpha->value  = inAlpha;

		// send values to video server via nanomsg
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgAlpha, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLAlpha_Dtor(GLAlpha *unit)
{
	RTFree(unit->mWorld, unit->msgAlpha);
}

//////////////////////////////////////////////////////////////////////////////////

void GLOpacity_Ctor(GLOpacity *unit)
{
	SETCALC(GLOpacity_next_k);
	unit->rateDivideCounter = 0;

	unit->msgOpacity = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgOpacity->msgType = 0;
	GLOpacity_next_k(unit, 1);
}

void GLOpacity_next_k(GLOpacity *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		float inOpacity = ZIN0(1); // get first sample

		// prepare messages to send to the video server
		unit->msgOpacity->nodeID = unit->mParent->mNode.mID;
		unit->msgOpacity->unitID = unit->mUnitIndex;
		unit->msgOpacity->index  = 1;
		unit->msgOpacity->value  = inOpacity;

		// send values to video server via nanomsg
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgOpacity, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLOpacity_Dtor(GLOpacity *unit)
{
	RTFree(unit->mWorld, unit->msgOpacity);
}

//////////////////////////////////////////////////////////////////////////////////

void GLWhite_Ctor(GLWhite *unit)
{
	SETCALC(GLWhite_next_k);
	GLWhite_next_k(unit, 1);
}

void GLWhite_next_k(GLWhite *unit, int inNumSamples)
{
	ZOUT0(0) = 0.f; // always output zero
}

//////////////////////////////////////////////////////////////////////////////////

void GLOut_Ctor(GLOut *unit)
{
	SETCALC(GLOut_next_k);
	GLOut_next_k(unit, 1);
}

void GLOut_next_k(GLOut *unit, int inNumSamples)
{
	ZOUT0(0) = 0.f; // always output zero
}

//////////////////////////////////////////////////////////////////////////////////

void GLPlayImg_Ctor(GLPlayImg *unit)
{
	SETCALC(GLPlayImg_next_k);
	unit->rateDivideCounter = 0;

	unit->msgImgID = (ImageMsg *)RTAlloc(unit->mWorld, sizeof(ImageMsg));
	unit->msgImgID->msgType = 2;
	GLPlayImg_next_k(unit, 1);
}

void GLPlayImg_next_k(GLPlayImg *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		// int32_t inImgID = (int32_t) DEMANDINPUT(0);
		int32_t inImgID = (int32_t) ZIN0(0);

		// prepare messages to send to the video server
		unit->msgImgID->nodeID  = unit->mParent->mNode.mID;
		unit->msgImgID->unitID  = unit->mUnitIndex;
		unit->msgImgID->index   = 0;
		unit->msgImgID->imageID = inImgID;

		// send values to video server via nanomsg
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgImgID, sizeof(ImageMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLPlayImg_Dtor(GLPlayImg *unit)
{
	RTFree(unit->mWorld, unit->msgImgID);
}

//////////////////////////////////////////////////////////////////////////////////

void GLPlayVid_Ctor(GLPlayVid *unit)
{
	SETCALC(GLPlayVid_next_k);
	unit->rateDivideCounter = 0;

	unit->msgVidID = (VideoMsg *)RTAlloc(unit->mWorld, sizeof(VideoMsg));
	unit->msgVidID->msgType = 1;
	GLPlayVid_next_k(unit, 1);
}

void GLPlayVid_next_k(GLPlayVid *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		// int32_t inVidID = (int32_t) DEMANDINPUT(0);
		int32_t inVidID = (int32_t) ZIN0(0);

		// prepare messages to send to the video server
		unit->msgVidID->nodeID  = unit->mParent->mNode.mID;
		unit->msgVidID->unitID  = unit->mUnitIndex;
		unit->msgVidID->index   = 0;
		unit->msgVidID->videoID = inVidID;

		// send values to video server via nanomsg
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgVidID, sizeof(VideoMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLPlayVid_Dtor(GLPlayVid *unit)
{
	RTFree(unit->mWorld, unit->msgVidID);
}

//////////////////////////////////////////////////////////////////////////////////

void GLPrevFrame_Ctor(GLPrevFrame *unit)
{
	SETCALC(GLPrevFrame_next_k);
	GLPrevFrame_next_k(unit, 1);
}

void GLPrevFrame_next_k(GLPrevFrame *unit, int inNumSamples)
{
	ZOUT0(0) = 0.f; // always output zero
}

//////////////////////////////////////////////////////////////////////////////////

void GLPrevFrame2_Ctor(GLPrevFrame2 *unit)
{
	SETCALC(GLPrevFrame2_next_k);
	GLPrevFrame2_next_k(unit, 1);
}

void GLPrevFrame2_next_k(GLPrevFrame2 *unit, int inNumSamples)
{
	ZOUT0(0) = 0.f; // always output zero
}

//////////////////////////////////////////////////////////////////////////////////

void GLRGB_Ctor(GLRGB *unit)
{
	SETCALC(GLRGB_next_k);
	unit->rateDivideCounter = 0;

	unit->msgR = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgR->msgType = 0;
	unit->msgG = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgG->msgType = 0;
	unit->msgB = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgB->msgType = 0;
	GLRGB_next_k(unit, 1);
}

void GLRGB_next_k(GLRGB *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		unit->msgR->nodeID = unit->mParent->mNode.mID;
		unit->msgR->unitID = unit->mUnitIndex;
		unit->msgR->index  = 1;
		unit->msgR->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgR, sizeof(DataMsg), 0);

		unit->msgG->nodeID = unit->mParent->mNode.mID;
		unit->msgG->unitID = unit->mUnitIndex;
		unit->msgG->index  = 2;
		unit->msgG->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgG, sizeof(DataMsg), 0);

		unit->msgB->nodeID = unit->mParent->mNode.mID;
		unit->msgB->unitID = unit->mUnitIndex;
		unit->msgB->index  = 3;
		unit->msgB->value  = ZIN0(3);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgB, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLRGB_Dtor(GLRGB *unit)
{
	RTFree(unit->mWorld, unit->msgR);
	RTFree(unit->mWorld, unit->msgG);
	RTFree(unit->mWorld, unit->msgB);
}

//////////////////////////////////////////////////////////////////////////////////

void GLRGBA_Ctor(GLRGBA *unit)
{
	SETCALC(GLRGBA_next_k);
	unit->rateDivideCounter = 0;

	unit->msgR = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgR->msgType = 0;
	unit->msgG = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgG->msgType = 0;
	unit->msgB = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgB->msgType = 0;
	unit->msgA = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgA->msgType = 0;
	GLRGBA_next_k(unit, 1);
}

void GLRGBA_next_k(GLRGBA *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		unit->msgR->nodeID = unit->mParent->mNode.mID;
		unit->msgR->unitID = unit->mUnitIndex;
		unit->msgR->index  = 1;
		unit->msgR->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgR, sizeof(DataMsg), 0);

		unit->msgG->nodeID = unit->mParent->mNode.mID;
		unit->msgG->unitID = unit->mUnitIndex;
		unit->msgG->index  = 2;
		unit->msgG->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgG, sizeof(DataMsg), 0);

		unit->msgB->nodeID = unit->mParent->mNode.mID;
		unit->msgB->unitID = unit->mUnitIndex;
		unit->msgB->index  = 3;
		unit->msgB->value  = ZIN0(3);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgB, sizeof(DataMsg), 0);

		unit->msgA->nodeID = unit->mParent->mNode.mID;
		unit->msgA->unitID = unit->mUnitIndex;
		unit->msgA->index  = 4;
		unit->msgA->value  = ZIN0(4);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgA, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLRGBA_Dtor(GLRGBA *unit)
{
	RTFree(unit->mWorld, unit->msgR);
	RTFree(unit->mWorld, unit->msgG);
	RTFree(unit->mWorld, unit->msgB);
	RTFree(unit->mWorld, unit->msgA);
}

//////////////////////////////////////////////////////////////////////////////////

void GLMix_Ctor(GLMix *unit)
{
	SETCALC(GLMix_next_k);
	unit->rateDivideCounter = 0;

	unit->msgMix = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgMix->msgType = 0;
	GLMix_next_k(unit, 1);
}

void GLMix_next_k(GLMix *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msgMix->nodeID = unit->mParent->mNode.mID;
		unit->msgMix->unitID = unit->mUnitIndex;
		unit->msgMix->index  = 2;
		unit->msgMix->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgMix, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLMix_Dtor(GLMix *unit)
{
	RTFree(unit->mWorld, unit->msgMix);
}

//////////////////////////////////////////////////////////////////////////////////

void GLBlend_Ctor(GLBlend *unit)
{
	SETCALC(GLBlend_next_k);
	unit->rateDivideCounter = 0;

	unit->msgBlendMode = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgBlendMode->msgType = 0;
	unit->msgMix = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgMix->msgType = 0;
	GLBlend_next_k(unit, 1);
}

void GLBlend_next_k(GLBlend *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msgBlendMode->nodeID = unit->mParent->mNode.mID;
		unit->msgBlendMode->unitID = unit->mUnitIndex;
		unit->msgBlendMode->index  = 2;
		unit->msgBlendMode->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgBlendMode, sizeof(DataMsg), 0);

		unit->msgMix->nodeID = unit->mParent->mNode.mID;
		unit->msgMix->unitID = unit->mUnitIndex;
		unit->msgMix->index  = 3;
		unit->msgMix->value  = ZIN0(3);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgMix, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLBlend_Dtor(GLBlend *unit)
{
	RTFree(unit->mWorld, unit->msgBlendMode);
	RTFree(unit->mWorld, unit->msgMix);
}

//////////////////////////////////////////////////////////////////////////////////

void GLShowImgTex_Ctor(GLShowImgTex *unit)
{
	SETCALC(GLShowImgTex_next_k);
	unit->rateDivideCounter = 0;

	unit->msgImgID = (ImageMsg *)RTAlloc(unit->mWorld, sizeof(ImageMsg));
	unit->msgImgID->msgType = 2;
	GLShowImgTex_next_k(unit, 1);
}

void GLShowImgTex_next_k(GLShowImgTex *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		// int32_t inImgID = (int32_t) DEMANDINPUT(0);
		int32_t inImgID = (int32_t) ZIN0(0);

		// prepare messages to send to the video server
		unit->msgImgID->nodeID  = unit->mParent->mNode.mID;
		unit->msgImgID->unitID  = unit->mUnitIndex;
		unit->msgImgID->index   = 0;
		unit->msgImgID->imageID = inImgID;

		// send values to video server via nanomsg
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgImgID, sizeof(ImageMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLShowImgTex_Dtor(GLShowImgTex *unit)
{
	RTFree(unit->mWorld, unit->msgImgID);
}

//////////////////////////////////////////////////////////////////////////////////

void GLShowVidTex_Ctor(GLShowVidTex *unit)
{
	SETCALC(GLShowVidTex_next_k);
	unit->rateDivideCounter = 0;

	unit->msgVidID = (VideoMsg *)RTAlloc(unit->mWorld, sizeof(VideoMsg));
	unit->msgVidID->msgType = 1;
	GLShowVidTex_next_k(unit, 1);
}

void GLShowVidTex_next_k(GLShowVidTex *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		// int32_t inVidID = (int32_t) DEMANDINPUT(0);
		int32_t inVidID = (int32_t) ZIN0(0);

		// prepare messages to send to the video server
		unit->msgVidID->nodeID  = unit->mParent->mNode.mID;
		unit->msgVidID->unitID  = unit->mUnitIndex;
		unit->msgVidID->index   = 0;
		unit->msgVidID->videoID = inVidID;

		// send values to video server via nanomsg
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgVidID, sizeof(VideoMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLShowVidTex_Dtor(GLShowVidTex *unit)
{
	RTFree(unit->mWorld, unit->msgVidID);
}

//////////////////////////////////////////////////////////////////////////////////

void GLTexCoords_Ctor(GLTexCoords *unit)
{
	SETCALC(GLTexCoords_next_k);
	GLTexCoords_next_k(unit, 1);
}

void GLTexCoords_next_k(GLTexCoords *unit, int inNumSamples)
{
	ZOUT0(0) = 0.f; // always output zero
}

//////////////////////////////////////////////////////////////////////////////////

void GLTexFlipX_Ctor(GLTexFlipX *unit)
{
	SETCALC(GLTexFlipX_next_k);
	GLTexFlipX_next_k(unit, 1);
}

void GLTexFlipX_next_k(GLTexFlipX *unit, int inNumSamples)
{
	ZOUT0(0) = 0.f; // always output zero
}

//////////////////////////////////////////////////////////////////////////////////

void GLTexFlipY_Ctor(GLTexFlipY *unit)
{
	SETCALC(GLTexFlipY_next_k);
	GLTexFlipY_next_k(unit, 1);
}

void GLTexFlipY_next_k(GLTexFlipY *unit, int inNumSamples)
{
	ZOUT0(0) = 0.f; // always output zero
}

//////////////////////////////////////////////////////////////////////////////////

void GLTexMirrorX_Ctor(GLTexMirrorX *unit)
{
	SETCALC(GLTexMirrorX_next_k);
	unit->rateDivideCounter = 0;

	unit->msgMirrorX = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgMirrorX->msgType = 0;
	unit->msgPolarity = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgPolarity->msgType = 0;
	GLTexMirrorX_next_k(unit, 1);
}

void GLTexMirrorX_next_k(GLTexMirrorX *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msgMirrorX->nodeID = unit->mParent->mNode.mID;
		unit->msgMirrorX->unitID = unit->mUnitIndex;
		unit->msgMirrorX->index  = 1;
		unit->msgMirrorX->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgMirrorX, sizeof(DataMsg), 0);

		unit->msgPolarity->nodeID = unit->mParent->mNode.mID;
		unit->msgPolarity->unitID = unit->mUnitIndex;
		unit->msgPolarity->index  = 2;
		unit->msgPolarity->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgPolarity, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLTexMirrorX_Dtor(GLTexMirrorX *unit)
{
	RTFree(unit->mWorld, unit->msgMirrorX);
	RTFree(unit->mWorld, unit->msgPolarity);
}

//////////////////////////////////////////////////////////////////////////////////

void GLTexMirrorY_Ctor(GLTexMirrorY *unit)
{
	SETCALC(GLTexMirrorY_next_k);
	unit->rateDivideCounter = 0;

	unit->msgMirrorY = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgMirrorY->msgType = 0;
	unit->msgPolarity = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgPolarity->msgType = 0;
	GLTexMirrorY_next_k(unit, 1);
}

void GLTexMirrorY_next_k(GLTexMirrorY *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msgMirrorY->nodeID = unit->mParent->mNode.mID;
		unit->msgMirrorY->unitID = unit->mUnitIndex;
		unit->msgMirrorY->index  = 1;
		unit->msgMirrorY->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgMirrorY, sizeof(DataMsg), 0);

		unit->msgPolarity->nodeID = unit->mParent->mNode.mID;
		unit->msgPolarity->unitID = unit->mUnitIndex;
		unit->msgPolarity->index  = 2;
		unit->msgPolarity->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgPolarity, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLTexMirrorY_Dtor(GLTexMirrorY *unit)
{
	RTFree(unit->mWorld, unit->msgMirrorY);
	RTFree(unit->mWorld, unit->msgPolarity);
}

//////////////////////////////////////////////////////////////////////////////////

void GLTexScale_Ctor(GLTexScale *unit)
{
	SETCALC(GLTexScale_next_k);
	unit->rateDivideCounter = 0;

	unit->msgScaleX = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgScaleX->msgType = 0;
	unit->msgScaleY = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgScaleY->msgType = 0;
	GLTexScale_next_k(unit, 1);
}

void GLTexScale_next_k(GLTexScale *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msgScaleX->nodeID = unit->mParent->mNode.mID;
		unit->msgScaleX->unitID = unit->mUnitIndex;
		unit->msgScaleX->index  = 1;
		unit->msgScaleX->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgScaleX, sizeof(DataMsg), 0);

		unit->msgScaleY->nodeID = unit->mParent->mNode.mID;
		unit->msgScaleY->unitID = unit->mUnitIndex;
		unit->msgScaleY->index  = 2;
		unit->msgScaleY->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgScaleY, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLTexScale_Dtor(GLTexScale *unit)
{
	RTFree(unit->mWorld, unit->msgScaleX);
	RTFree(unit->mWorld, unit->msgScaleY);
}

//////////////////////////////////////////////////////////////////////////////////

void GLTexTrans_Ctor(GLTexTrans *unit)
{
	SETCALC(GLTexTrans_next_k);
	unit->rateDivideCounter = 0;

	unit->msgTransX = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgTransX->msgType = 0;
	unit->msgTransY = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgTransY->msgType = 0;
	GLTexTrans_next_k(unit, 1);
}

void GLTexTrans_next_k(GLTexTrans *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msgTransX->nodeID = unit->mParent->mNode.mID;
		unit->msgTransX->unitID = unit->mUnitIndex;
		unit->msgTransX->index  = 1;
		unit->msgTransX->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgTransX, sizeof(DataMsg), 0);

		unit->msgTransY->nodeID = unit->mParent->mNode.mID;
		unit->msgTransY->unitID = unit->mUnitIndex;
		unit->msgTransY->index  = 2;
		unit->msgTransY->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgTransY, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLTexTrans_Dtor(GLTexTrans *unit)
{
	RTFree(unit->mWorld, unit->msgTransX);
	RTFree(unit->mWorld, unit->msgTransY);
}

//////////////////////////////////////////////////////////////////////////////////

void GLRotate_Ctor(GLRotate *unit)
{
	SETCALC(GLRotate_next_k);
	unit->rateDivideCounter = 0;

	unit->msgAngle = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgAngle->msgType = 0;
	GLRotate_next_k(unit, 1);
}

void GLRotate_next_k(GLRotate *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msgAngle->nodeID = unit->mParent->mNode.mID;
		unit->msgAngle->unitID = unit->mUnitIndex;
		unit->msgAngle->index  = 1;
		unit->msgAngle->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgAngle, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLRotate_Dtor(GLRotate *unit)
{
	RTFree(unit->mWorld, unit->msgAngle);
}

//////////////////////////////////////////////////////////////////////////////////

void SHKCheckerboard_Ctor(SHKCheckerboard *unit)
{
	SETCALC(SHKCheckerboard_next_k);
	unit->rateDivideCounter = 0;

	unit->msgRows = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgRows->msgType = 0;
	unit->msgCols = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgCols->msgType = 0;
	SHKCheckerboard_next_k(unit, 1);
}

void SHKCheckerboard_next_k(SHKCheckerboard *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		float inRows = ZIN0(1);
		float inCols = ZIN0(2);

		unit->msgRows->nodeID = unit->mParent->mNode.mID;
		unit->msgRows->unitID = unit->mUnitIndex;
		unit->msgRows->index  = 1;
		unit->msgRows->value  = inRows;
		unit->msgCols->nodeID = unit->mParent->mNode.mID;
		unit->msgCols->unitID = unit->mUnitIndex;
		unit->msgCols->index  = 2;
		unit->msgCols->value  = inCols;

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgRows, sizeof(DataMsg), 0);
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgCols, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void SHKCheckerboard_Dtor(SHKCheckerboard *unit)
{
	RTFree(unit->mWorld, unit->msgRows);
	RTFree(unit->mWorld, unit->msgCols);
}

//////////////////////////////////////////////////////////////////////////////////

void SHKCircleWave_Ctor(SHKCircleWave *unit)
{
	SETCALC(SHKCircleWave_next_k);
	unit->rateDivideCounter = 0;

	unit->msgTime = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgTime->msgType = 0;
	unit->msgSpeed = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgSpeed->msgType = 0;
	unit->msgBrightness = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgBrightness->msgType = 0;
	unit->msgStrength = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgStrength->msgType = 0;
	unit->msgDensity = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgDensity->msgType = 0;
	unit->msgCenter = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgCenter->msgType = 0;

	SHKCircleWave_next_k(unit, 1);
}

void SHKCircleWave_next_k(SHKCircleWave *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		float inTime = ZIN0(1);
		float inSpeed = ZIN0(2);
		float inBrightness = ZIN0(3);
		float inStrength = ZIN0(4);
		float inDensity = ZIN0(5);
		float inCenter = ZIN0(6);

		unit->msgTime->nodeID = unit->mParent->mNode.mID;
		unit->msgTime->unitID = unit->mUnitIndex;
		unit->msgTime->index  = 1;
		unit->msgTime->value  = inTime;
		unit->msgSpeed->nodeID = unit->mParent->mNode.mID;
		unit->msgSpeed->unitID = unit->mUnitIndex;
		unit->msgSpeed->index  = 2;
		unit->msgSpeed->value  = inSpeed;
		unit->msgBrightness->nodeID = unit->mParent->mNode.mID;
		unit->msgBrightness->unitID = unit->mUnitIndex;
		unit->msgBrightness->index  = 3;
		unit->msgBrightness->value  = inBrightness;
		unit->msgStrength->nodeID = unit->mParent->mNode.mID;
		unit->msgStrength->unitID = unit->mUnitIndex;
		unit->msgStrength->index  = 4;
		unit->msgStrength->value  = inStrength;
		unit->msgDensity->nodeID = unit->mParent->mNode.mID;
		unit->msgDensity->unitID = unit->mUnitIndex;
		unit->msgDensity->index  = 5;
		unit->msgDensity->value  = inDensity;
		unit->msgCenter->nodeID = unit->mParent->mNode.mID;
		unit->msgCenter->unitID = unit->mUnitIndex;
		unit->msgCenter->index  = 6;
		unit->msgCenter->value  = inCenter;

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgTime, sizeof(DataMsg), 0);
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgSpeed, sizeof(DataMsg), 0);
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgBrightness, sizeof(DataMsg), 0);
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgStrength, sizeof(DataMsg), 0);
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgDensity, sizeof(DataMsg), 0);
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgCenter, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void SHKCircleWave_Dtor(SHKCircleWave *unit)
{
	RTFree(unit->mWorld, unit->msgTime);
	RTFree(unit->mWorld, unit->msgSpeed);
	RTFree(unit->mWorld, unit->msgBrightness);
	RTFree(unit->mWorld, unit->msgStrength);
	RTFree(unit->mWorld, unit->msgDensity);
	RTFree(unit->mWorld, unit->msgCenter);
}

//////////////////////////////////////////////////////////////////////////////////

void SHKColorInvert_Ctor(SHKColorInvert *unit)
{
	SETCALC(SHKColorInvert_next_k);
	SHKColorInvert_next_k(unit, 1);
}

void SHKColorInvert_next_k(SHKColorInvert *unit, int inNumSamples)
{
	ZOUT0(0) = 0.f; // always output zero
}

//////////////////////////////////////////////////////////////////////////////////

void SHKDesaturate_Ctor(SHKDesaturate *unit)
{
	SETCALC(SHKDesaturate_next_k);
	unit->rateDivideCounter = 0;

	unit->msgStrength = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgStrength->msgType = 0;

	SHKDesaturate_next_k(unit, 1);
}

void SHKDesaturate_next_k(SHKDesaturate *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		float inStrength = ZIN0(1);

		unit->msgStrength->nodeID = unit->mParent->mNode.mID;
		unit->msgStrength->unitID = unit->mUnitIndex;
		unit->msgStrength->index  = 1;
		unit->msgStrength->value  = inStrength;

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgStrength, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void SHKDesaturate_Dtor(SHKDesaturate *unit)
{
	RTFree(unit->mWorld, unit->msgStrength);
}


//////////////////////////////////////////////////////////////////////////////////

void SHKWater_Ctor(SHKWater *unit)
{
	SETCALC(SHKWater_next_k);
	unit->rateDivideCounter = 0;

	unit->msgVidID = (VideoMsg *)RTAlloc(unit->mWorld, sizeof(VideoMsg));
	unit->msgVidID->msgType = 1;
	unit->msgTime = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgTime->msgType = 0;
	unit->msgSpeed = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgSpeed->msgType = 0;
	unit->msgStrength = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgStrength->msgType = 0;
	unit->msgFreq = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msgFreq->msgType = 0;

	SHKWater_next_k(unit, 1);
}

void SHKWater_next_k(SHKWater *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		int32_t inVidID = (int32_t)ZIN0(0);
		float inTime = ZIN0(1);
		float inSpeed = ZIN0(2);
		float inStrength = ZIN0(3);
		float inFreq = ZIN0(4);

		// prepare messages to send to the video server
		unit->msgVidID->nodeID = unit->mParent->mNode.mID;
		unit->msgVidID->unitID = unit->mUnitIndex;
		unit->msgVidID->index  = 0;
		unit->msgVidID->videoID = inVidID;
		unit->msgTime->nodeID = unit->mParent->mNode.mID;
		unit->msgTime->unitID = unit->mUnitIndex;
		unit->msgTime->index  = 1;
		unit->msgTime->value  = inTime;
		unit->msgSpeed->nodeID = unit->mParent->mNode.mID;
		unit->msgSpeed->unitID = unit->mUnitIndex;
		unit->msgSpeed->index  = 2;
		unit->msgSpeed->value  = inTime;
		unit->msgStrength->nodeID = unit->mParent->mNode.mID;
		unit->msgStrength->unitID = unit->mUnitIndex;
		unit->msgStrength->index  = 3;
		unit->msgStrength->value  = inTime;
		unit->msgFreq->nodeID = unit->mParent->mNode.mID;
		unit->msgFreq->unitID = unit->mUnitIndex;
		unit->msgFreq->index  = 4;
		unit->msgFreq->value  = inTime;

		zmq_send(unit->mWorld->mDataMsgSock, unit->msgTime, sizeof(DataMsg), 0);
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgSpeed, sizeof(DataMsg), 0);
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgStrength, sizeof(DataMsg), 0);
		zmq_send(unit->mWorld->mDataMsgSock, unit->msgFreq, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void SHKWater_Dtor(SHKWater *unit)
{
	RTFree(unit->mWorld, unit->msgTime);
	RTFree(unit->mWorld, unit->msgSpeed);
	RTFree(unit->mWorld, unit->msgStrength);
	RTFree(unit->mWorld, unit->msgFreq);
}

//////////////////////////////////////////////////////////////////////////////////

void GLFunc1_Ctor(GLFunc1 *unit)
{
	SETCALC(GLFunc1_next_k);
	unit->rateDivideCounter = 0;

	unit->msg1 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg1->msgType = 0;
	GLFunc1_next_k(unit, 1);
}

void GLFunc1_next_k(GLFunc1 *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msg1->nodeID = unit->mParent->mNode.mID;
		unit->msg1->unitID = unit->mUnitIndex;
		unit->msg1->index  = 0;
		unit->msg1->value  = ZIN0(0);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg1, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLFunc1_Dtor(GLFunc1 *unit)
{
	RTFree(unit->mWorld, unit->msg1);
}

//////////////////////////////////////////////////////////////////////////////////

void GLFunc2_Ctor(GLFunc2 *unit)
{
	SETCALC(GLFunc2_next_k);
	unit->rateDivideCounter = 0;

	unit->msg1 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg1->msgType = 0;
	unit->msg2 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg2->msgType = 0;
	GLFunc2_next_k(unit, 1);
}

void GLFunc2_next_k(GLFunc2 *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msg1->nodeID = unit->mParent->mNode.mID;
		unit->msg1->unitID = unit->mUnitIndex;
		unit->msg1->index  = 0;
		unit->msg1->value  = ZIN0(0);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg1, sizeof(DataMsg), 0);

		unit->msg2->nodeID = unit->mParent->mNode.mID;
		unit->msg2->unitID = unit->mUnitIndex;
		unit->msg2->index  = 1;
		unit->msg2->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg2, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLFunc2_Dtor(GLFunc2 *unit)
{
	RTFree(unit->mWorld, unit->msg1);
	RTFree(unit->mWorld, unit->msg2);
}

//////////////////////////////////////////////////////////////////////////////////

void GLFunc3_Ctor(GLFunc3 *unit)
{
	SETCALC(GLFunc3_next_k);
	unit->rateDivideCounter = 0;

	unit->msg1 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg1->msgType = 0;
	unit->msg2 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg2->msgType = 0;
	unit->msg3 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg3->msgType = 0;
	GLFunc3_next_k(unit, 1);
}

void GLFunc3_next_k(GLFunc3 *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msg1->nodeID = unit->mParent->mNode.mID;
		unit->msg1->unitID = unit->mUnitIndex;
		unit->msg1->index  = 0;
		unit->msg1->value  = ZIN0(0);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg1, sizeof(DataMsg), 0);

		unit->msg2->nodeID = unit->mParent->mNode.mID;
		unit->msg2->unitID = unit->mUnitIndex;
		unit->msg2->index  = 1;
		unit->msg2->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg2, sizeof(DataMsg), 0);

		unit->msg3->nodeID = unit->mParent->mNode.mID;
		unit->msg3->unitID = unit->mUnitIndex;
		unit->msg3->index  = 2;
		unit->msg3->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg3, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLFunc3_Dtor(GLFunc3 *unit)
{
	RTFree(unit->mWorld, unit->msg1);
	RTFree(unit->mWorld, unit->msg2);
	RTFree(unit->mWorld, unit->msg3);
}

//////////////////////////////////////////////////////////////////////////////////

void GLFunc4_Ctor(GLFunc4 *unit)
{
	SETCALC(GLFunc4_next_k);
	unit->rateDivideCounter = 0;

	unit->msg1 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg1->msgType = 0;
	unit->msg2 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg2->msgType = 0;
	unit->msg3 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg3->msgType = 0;
	unit->msg4 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg4->msgType = 0;
	GLFunc4_next_k(unit, 1);
}

void GLFunc4_next_k(GLFunc4 *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msg1->nodeID = unit->mParent->mNode.mID;
		unit->msg1->unitID = unit->mUnitIndex;
		unit->msg1->index  = 0;
		unit->msg1->value  = ZIN0(0);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg1, sizeof(DataMsg), 0);

		unit->msg2->nodeID = unit->mParent->mNode.mID;
		unit->msg2->unitID = unit->mUnitIndex;
		unit->msg2->index  = 1;
		unit->msg2->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg2, sizeof(DataMsg), 0);

		unit->msg3->nodeID = unit->mParent->mNode.mID;
		unit->msg3->unitID = unit->mUnitIndex;
		unit->msg3->index  = 2;
		unit->msg3->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg3, sizeof(DataMsg), 0);

		unit->msg4->nodeID = unit->mParent->mNode.mID;
		unit->msg4->unitID = unit->mUnitIndex;
		unit->msg4->index  = 3;
		unit->msg4->value  = ZIN0(3);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg4, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLFunc4_Dtor(GLFunc4 *unit)
{
	RTFree(unit->mWorld, unit->msg1);
	RTFree(unit->mWorld, unit->msg2);
	RTFree(unit->mWorld, unit->msg3);
	RTFree(unit->mWorld, unit->msg4);
}

//////////////////////////////////////////////////////////////////////////////////

void GLFunc5_Ctor(GLFunc5 *unit)
{
	SETCALC(GLFunc5_next_k);
	unit->rateDivideCounter = 0;

	unit->msg1 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg1->msgType = 0;
	unit->msg2 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg2->msgType = 0;
	unit->msg3 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg3->msgType = 0;
	unit->msg4 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg4->msgType = 0;
	unit->msg5 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg5->msgType = 0;
	GLFunc5_next_k(unit, 1);
}

void GLFunc5_next_k(GLFunc5 *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msg1->nodeID = unit->mParent->mNode.mID;
		unit->msg1->unitID = unit->mUnitIndex;
		unit->msg1->index  = 0;
		unit->msg1->value  = ZIN0(0);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg1, sizeof(DataMsg), 0);

		unit->msg2->nodeID = unit->mParent->mNode.mID;
		unit->msg2->unitID = unit->mUnitIndex;
		unit->msg2->index  = 1;
		unit->msg2->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg2, sizeof(DataMsg), 0);

		unit->msg3->nodeID = unit->mParent->mNode.mID;
		unit->msg3->unitID = unit->mUnitIndex;
		unit->msg3->index  = 2;
		unit->msg3->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg3, sizeof(DataMsg), 0);

		unit->msg4->nodeID = unit->mParent->mNode.mID;
		unit->msg4->unitID = unit->mUnitIndex;
		unit->msg4->index  = 3;
		unit->msg4->value  = ZIN0(3);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg4, sizeof(DataMsg), 0);

		unit->msg5->nodeID = unit->mParent->mNode.mID;
		unit->msg5->unitID = unit->mUnitIndex;
		unit->msg5->index  = 4;
		unit->msg5->value  = ZIN0(4);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg5, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLFunc5_Dtor(GLFunc5 *unit)
{
	RTFree(unit->mWorld, unit->msg1);
	RTFree(unit->mWorld, unit->msg2);
	RTFree(unit->mWorld, unit->msg3);
	RTFree(unit->mWorld, unit->msg4);
	RTFree(unit->mWorld, unit->msg5);
}

//////////////////////////////////////////////////////////////////////////////////

void GLFunc6_Ctor(GLFunc6 *unit)
{
	SETCALC(GLFunc6_next_k);
	unit->rateDivideCounter = 0;

	unit->msg1 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg1->msgType = 0;
	unit->msg2 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg2->msgType = 0;
	unit->msg3 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg3->msgType = 0;
	unit->msg4 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg4->msgType = 0;
	unit->msg5 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg5->msgType = 0;
	unit->msg6 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg6->msgType = 0;
	GLFunc6_next_k(unit, 1);
}

void GLFunc6_next_k(GLFunc6 *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msg1->nodeID = unit->mParent->mNode.mID;
		unit->msg1->unitID = unit->mUnitIndex;
		unit->msg1->index  = 0;
		unit->msg1->value  = ZIN0(0);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg1, sizeof(DataMsg), 0);

		unit->msg2->nodeID = unit->mParent->mNode.mID;
		unit->msg2->unitID = unit->mUnitIndex;
		unit->msg2->index  = 1;
		unit->msg2->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg2, sizeof(DataMsg), 0);

		unit->msg3->nodeID = unit->mParent->mNode.mID;
		unit->msg3->unitID = unit->mUnitIndex;
		unit->msg3->index  = 2;
		unit->msg3->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg3, sizeof(DataMsg), 0);

		unit->msg4->nodeID = unit->mParent->mNode.mID;
		unit->msg4->unitID = unit->mUnitIndex;
		unit->msg4->index  = 3;
		unit->msg4->value  = ZIN0(3);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg4, sizeof(DataMsg), 0);

		unit->msg5->nodeID = unit->mParent->mNode.mID;
		unit->msg5->unitID = unit->mUnitIndex;
		unit->msg5->index  = 4;
		unit->msg5->value  = ZIN0(4);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg5, sizeof(DataMsg), 0);

		unit->msg6->nodeID = unit->mParent->mNode.mID;
		unit->msg6->unitID = unit->mUnitIndex;
		unit->msg6->index  = 5;
		unit->msg6->value  = ZIN0(5);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg6, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLFunc6_Dtor(GLFunc6 *unit)
{
	RTFree(unit->mWorld, unit->msg1);
	RTFree(unit->mWorld, unit->msg2);
	RTFree(unit->mWorld, unit->msg3);
	RTFree(unit->mWorld, unit->msg4);
	RTFree(unit->mWorld, unit->msg5);
	RTFree(unit->mWorld, unit->msg6);
}

//////////////////////////////////////////////////////////////////////////////////

void GLFunc7_Ctor(GLFunc7 *unit)
{
	SETCALC(GLFunc7_next_k);
	unit->rateDivideCounter = 0;

	unit->msg1 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg1->msgType = 0;
	unit->msg2 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg2->msgType = 0;
	unit->msg3 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg3->msgType = 0;
	unit->msg4 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg4->msgType = 0;
	unit->msg5 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg5->msgType = 0;
	unit->msg6 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg6->msgType = 0;
	unit->msg7 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg7->msgType = 0;
	GLFunc7_next_k(unit, 1);
}

void GLFunc7_next_k(GLFunc7 *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msg1->nodeID = unit->mParent->mNode.mID;
		unit->msg1->unitID = unit->mUnitIndex;
		unit->msg1->index  = 0;
		unit->msg1->value  = ZIN0(0);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg1, sizeof(DataMsg), 0);

		unit->msg2->nodeID = unit->mParent->mNode.mID;
		unit->msg2->unitID = unit->mUnitIndex;
		unit->msg2->index  = 1;
		unit->msg2->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg2, sizeof(DataMsg), 0);

		unit->msg3->nodeID = unit->mParent->mNode.mID;
		unit->msg3->unitID = unit->mUnitIndex;
		unit->msg3->index  = 2;
		unit->msg3->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg3, sizeof(DataMsg), 0);

		unit->msg4->nodeID = unit->mParent->mNode.mID;
		unit->msg4->unitID = unit->mUnitIndex;
		unit->msg4->index  = 3;
		unit->msg4->value  = ZIN0(3);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg4, sizeof(DataMsg), 0);

		unit->msg5->nodeID = unit->mParent->mNode.mID;
		unit->msg5->unitID = unit->mUnitIndex;
		unit->msg5->index  = 4;
		unit->msg5->value  = ZIN0(4);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg5, sizeof(DataMsg), 0);

		unit->msg6->nodeID = unit->mParent->mNode.mID;
		unit->msg6->unitID = unit->mUnitIndex;
		unit->msg6->index  = 5;
		unit->msg6->value  = ZIN0(5);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg6, sizeof(DataMsg), 0);

		unit->msg7->nodeID = unit->mParent->mNode.mID;
		unit->msg7->unitID = unit->mUnitIndex;
		unit->msg7->index  = 6;
		unit->msg7->value  = ZIN0(6);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg7, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLFunc7_Dtor(GLFunc7 *unit)
{
	RTFree(unit->mWorld, unit->msg1);
	RTFree(unit->mWorld, unit->msg2);
	RTFree(unit->mWorld, unit->msg3);
	RTFree(unit->mWorld, unit->msg4);
	RTFree(unit->mWorld, unit->msg5);
	RTFree(unit->mWorld, unit->msg6);
	RTFree(unit->mWorld, unit->msg7);
}

//////////////////////////////////////////////////////////////////////////////////

void GLFunc8_Ctor(GLFunc8 *unit)
{
	SETCALC(GLFunc8_next_k);
	unit->rateDivideCounter = 0;

	unit->msg1 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg1->msgType = 0;
	unit->msg2 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg2->msgType = 0;
	unit->msg3 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg3->msgType = 0;
	unit->msg4 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg4->msgType = 0;
	unit->msg5 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg5->msgType = 0;
	unit->msg6 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg6->msgType = 0;
	unit->msg7 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg7->msgType = 0;
	unit->msg8 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg8->msgType = 0;
	GLFunc8_next_k(unit, 1);
}

void GLFunc8_next_k(GLFunc8 *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msg1->nodeID = unit->mParent->mNode.mID;
		unit->msg1->unitID = unit->mUnitIndex;
		unit->msg1->index  = 0;
		unit->msg1->value  = ZIN0(0);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg1, sizeof(DataMsg), 0);

		unit->msg2->nodeID = unit->mParent->mNode.mID;
		unit->msg2->unitID = unit->mUnitIndex;
		unit->msg2->index  = 1;
		unit->msg2->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg2, sizeof(DataMsg), 0);

		unit->msg3->nodeID = unit->mParent->mNode.mID;
		unit->msg3->unitID = unit->mUnitIndex;
		unit->msg3->index  = 2;
		unit->msg3->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg3, sizeof(DataMsg), 0);

		unit->msg4->nodeID = unit->mParent->mNode.mID;
		unit->msg4->unitID = unit->mUnitIndex;
		unit->msg4->index  = 3;
		unit->msg4->value  = ZIN0(3);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg4, sizeof(DataMsg), 0);

		unit->msg5->nodeID = unit->mParent->mNode.mID;
		unit->msg5->unitID = unit->mUnitIndex;
		unit->msg5->index  = 4;
		unit->msg5->value  = ZIN0(4);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg5, sizeof(DataMsg), 0);

		unit->msg6->nodeID = unit->mParent->mNode.mID;
		unit->msg6->unitID = unit->mUnitIndex;
		unit->msg6->index  = 5;
		unit->msg6->value  = ZIN0(5);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg6, sizeof(DataMsg), 0);

		unit->msg7->nodeID = unit->mParent->mNode.mID;
		unit->msg7->unitID = unit->mUnitIndex;
		unit->msg7->index  = 6;
		unit->msg7->value  = ZIN0(6);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg7, sizeof(DataMsg), 0);

		unit->msg8->nodeID = unit->mParent->mNode.mID;
		unit->msg8->unitID = unit->mUnitIndex;
		unit->msg8->index  = 7;
		unit->msg8->value  = ZIN0(7);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg8, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLFunc8_Dtor(GLFunc8 *unit)
{
	RTFree(unit->mWorld, unit->msg1);
	RTFree(unit->mWorld, unit->msg2);
	RTFree(unit->mWorld, unit->msg3);
	RTFree(unit->mWorld, unit->msg4);
	RTFree(unit->mWorld, unit->msg5);
	RTFree(unit->mWorld, unit->msg6);
	RTFree(unit->mWorld, unit->msg7);
	RTFree(unit->mWorld, unit->msg8);
}

//////////////////////////////////////////////////////////////////////////////////

void GLFunc9_Ctor(GLFunc9 *unit)
{
	SETCALC(GLFunc9_next_k);
	unit->rateDivideCounter = 0;

	unit->msg1 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg1->msgType = 0;
	unit->msg2 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg2->msgType = 0;
	unit->msg3 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg3->msgType = 0;
	unit->msg4 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg4->msgType = 0;
	unit->msg5 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg5->msgType = 0;
	unit->msg6 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg6->msgType = 0;
	unit->msg7 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg7->msgType = 0;
	unit->msg8 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg8->msgType = 0;
	unit->msg9 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg9->msgType = 0;
	GLFunc9_next_k(unit, 1);
}

void GLFunc9_next_k(GLFunc9 *unit, int inNumSamples)
{

	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msg1->nodeID = unit->mParent->mNode.mID;
		unit->msg1->unitID = unit->mUnitIndex;
		unit->msg1->index  = 0;
		unit->msg1->value  = ZIN0(0);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg1, sizeof(DataMsg), 0);

		unit->msg2->nodeID = unit->mParent->mNode.mID;
		unit->msg2->unitID = unit->mUnitIndex;
		unit->msg2->index  = 1;
		unit->msg2->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg2, sizeof(DataMsg), 0);

		unit->msg3->nodeID = unit->mParent->mNode.mID;
		unit->msg3->unitID = unit->mUnitIndex;
		unit->msg3->index  = 2;
		unit->msg3->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg3, sizeof(DataMsg), 0);

		unit->msg4->nodeID = unit->mParent->mNode.mID;
		unit->msg4->unitID = unit->mUnitIndex;
		unit->msg4->index  = 3;
		unit->msg4->value  = ZIN0(3);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg4, sizeof(DataMsg), 0);

		unit->msg5->nodeID = unit->mParent->mNode.mID;
		unit->msg5->unitID = unit->mUnitIndex;
		unit->msg5->index  = 4;
		unit->msg5->value  = ZIN0(4);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg5, sizeof(DataMsg), 0);

		unit->msg6->nodeID = unit->mParent->mNode.mID;
		unit->msg6->unitID = unit->mUnitIndex;
		unit->msg6->index  = 5;
		unit->msg6->value  = ZIN0(5);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg6, sizeof(DataMsg), 0);

		unit->msg7->nodeID = unit->mParent->mNode.mID;
		unit->msg7->unitID = unit->mUnitIndex;
		unit->msg7->index  = 6;
		unit->msg7->value  = ZIN0(6);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg7, sizeof(DataMsg), 0);

		unit->msg8->nodeID = unit->mParent->mNode.mID;
		unit->msg8->unitID = unit->mUnitIndex;
		unit->msg8->index  = 7;
		unit->msg8->value  = ZIN0(7);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg8, sizeof(DataMsg), 0);

		unit->msg9->nodeID = unit->mParent->mNode.mID;
		unit->msg9->unitID = unit->mUnitIndex;
		unit->msg9->index  = 8;
		unit->msg9->value  = ZIN0(8);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg9, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLFunc9_Dtor(GLFunc9 *unit)
{
	RTFree(unit->mWorld, unit->msg1);
	RTFree(unit->mWorld, unit->msg2);
	RTFree(unit->mWorld, unit->msg3);
	RTFree(unit->mWorld, unit->msg4);
	RTFree(unit->mWorld, unit->msg5);
	RTFree(unit->mWorld, unit->msg6);
	RTFree(unit->mWorld, unit->msg7);
	RTFree(unit->mWorld, unit->msg8);
	RTFree(unit->mWorld, unit->msg9);
}

//////////////////////////////////////////////////////////////////////////////////

void GLFunc10_Ctor(GLFunc10 *unit)
{
	SETCALC(GLFunc10_next_k);
	unit->rateDivideCounter = 0;

	unit->msg1 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg1->msgType = 0;
	unit->msg2 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg2->msgType = 0;
	unit->msg3 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg3->msgType = 0;
	unit->msg4 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg4->msgType = 0;
	unit->msg5 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg5->msgType = 0;
	unit->msg6 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg6->msgType = 0;
	unit->msg7 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg7->msgType = 0;
	unit->msg8 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg8->msgType = 0;
	unit->msg9 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg9->msgType = 0;
	unit->msg10 = (DataMsg *)RTAlloc(unit->mWorld, sizeof(DataMsg));
	unit->msg10->msgType = 0;
	GLFunc10_next_k(unit, 1);
}

void GLFunc10_next_k(GLFunc10 *unit, int inNumSamples)
{
	if (SHOULD_SEND) {
		// prepare messages to send to the video server
		unit->msg1->nodeID = unit->mParent->mNode.mID;
		unit->msg1->unitID = unit->mUnitIndex;
		unit->msg1->index  = 0;
		unit->msg1->value  = ZIN0(0);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg1, sizeof(DataMsg), 0);

		unit->msg2->nodeID = unit->mParent->mNode.mID;
		unit->msg2->unitID = unit->mUnitIndex;
		unit->msg2->index  = 1;
		unit->msg2->value  = ZIN0(1);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg2, sizeof(DataMsg), 0);

		unit->msg3->nodeID = unit->mParent->mNode.mID;
		unit->msg3->unitID = unit->mUnitIndex;
		unit->msg3->index  = 2;
		unit->msg3->value  = ZIN0(2);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg3, sizeof(DataMsg), 0);

		unit->msg4->nodeID = unit->mParent->mNode.mID;
		unit->msg4->unitID = unit->mUnitIndex;
		unit->msg4->index  = 3;
		unit->msg4->value  = ZIN0(3);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg4, sizeof(DataMsg), 0);

		unit->msg5->nodeID = unit->mParent->mNode.mID;
		unit->msg5->unitID = unit->mUnitIndex;
		unit->msg5->index  = 4;
		unit->msg5->value  = ZIN0(4);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg5, sizeof(DataMsg), 0);

		unit->msg6->nodeID = unit->mParent->mNode.mID;
		unit->msg6->unitID = unit->mUnitIndex;
		unit->msg6->index  = 5;
		unit->msg6->value  = ZIN0(5);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg6, sizeof(DataMsg), 0);

		unit->msg7->nodeID = unit->mParent->mNode.mID;
		unit->msg7->unitID = unit->mUnitIndex;
		unit->msg7->index  = 6;
		unit->msg7->value  = ZIN0(6);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg7, sizeof(DataMsg), 0);

		unit->msg8->nodeID = unit->mParent->mNode.mID;
		unit->msg8->unitID = unit->mUnitIndex;
		unit->msg8->index  = 7;
		unit->msg8->value  = ZIN0(7);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg8, sizeof(DataMsg), 0);

		unit->msg9->nodeID = unit->mParent->mNode.mID;
		unit->msg9->unitID = unit->mUnitIndex;
		unit->msg9->index  = 8;
		unit->msg9->value  = ZIN0(8);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg9, sizeof(DataMsg), 0);

		unit->msg10->nodeID = unit->mParent->mNode.mID;
		unit->msg10->unitID = unit->mUnitIndex;
		unit->msg10->index  = 9;
		unit->msg10->value  = ZIN0(9);

		zmq_send(unit->mWorld->mDataMsgSock, unit->msg10, sizeof(DataMsg), 0);
	}

	ZOUT0(0) = 0.f; // always output zero
}

void GLFunc10_Dtor(GLFunc10 *unit)
{
	RTFree(unit->mWorld, unit->msg1);
	RTFree(unit->mWorld, unit->msg2);
	RTFree(unit->mWorld, unit->msg3);
	RTFree(unit->mWorld, unit->msg4);
	RTFree(unit->mWorld, unit->msg5);
	RTFree(unit->mWorld, unit->msg6);
	RTFree(unit->mWorld, unit->msg7);
	RTFree(unit->mWorld, unit->msg8);
	RTFree(unit->mWorld, unit->msg9);
	RTFree(unit->mWorld, unit->msg10);
}

//////////////////////////////////////////////////////////////////////////////////

PluginLoad(Video)
{
	ft = inTable;

	DefineDtorUnit(GLRed);
	DefineDtorUnit(GLGreen);
	DefineDtorUnit(GLBlue);
	DefineDtorUnit(GLAlpha);
	DefineDtorUnit(GLOpacity);
	DefineSimpleUnit(GLWhite);
	DefineSimpleUnit(GLOut);
	DefineDtorUnit(GLPlayImg);
	DefineDtorUnit(GLPlayVid);
	DefineSimpleUnit(GLPrevFrame);
	DefineSimpleUnit(GLPrevFrame2);
	DefineDtorUnit(GLRGB);
	DefineDtorUnit(GLRGBA);
	DefineDtorUnit(GLMix);
	DefineDtorUnit(GLBlend);
	DefineDtorUnit(GLShowImgTex);
	DefineDtorUnit(GLShowVidTex);
	DefineSimpleUnit(GLTexCoords);
	DefineSimpleUnit(GLTexFlipX);
	DefineSimpleUnit(GLTexFlipY);
	DefineDtorUnit(GLTexMirrorX);
	DefineDtorUnit(GLTexMirrorY);
	DefineDtorUnit(GLTexScale);
	DefineDtorUnit(GLTexTrans);
	DefineDtorUnit(GLRotate);

	DefineDtorUnit(SHKCheckerboard);
	DefineDtorUnit(SHKCircleWave);
	DefineSimpleUnit(SHKColorInvert);
	DefineDtorUnit(SHKDesaturate);
	DefineDtorUnit(SHKWater);

	DefineDtorUnit(GLFunc1);
	DefineDtorUnit(GLFunc2);
	DefineDtorUnit(GLFunc3);
	DefineDtorUnit(GLFunc4);
	DefineDtorUnit(GLFunc5);
	DefineDtorUnit(GLFunc6);
	DefineDtorUnit(GLFunc7);
	DefineDtorUnit(GLFunc8);
	DefineDtorUnit(GLFunc9);
	DefineDtorUnit(GLFunc10);
}
