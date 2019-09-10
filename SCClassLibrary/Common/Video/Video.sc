VideoUGen : UGen {
    *new { arg ... args;
        ^this.new1( *args );
    }
}

GLRed : VideoUGen {
    *fr { arg in, red;
        ^this.new(\control, in, red)
    }
}

GLGreen : VideoUGen {
    *fr { arg in, green;
        ^this.new(\control, in, green)
    }
}

GLBlue : VideoUGen {
    *fr { arg in, blue;
        ^this.new(\control, in, blue)
    }
}

GLAlpha : VideoUGen {
    *fr { arg in, alpha;
        ^this.new(\control, in, alpha)
    }
}

GLOpacity : VideoUGen {
    *fr { arg in, opacity;
        ^this.new(\control, in, opacity)
    }
}

GLWhite : VideoUGen {
    *fr {
        ^this.new(\control)
    }
}

GLOut : VideoUGen {
    *fr { arg in;
        ^this.new(\control, in)
    }
}

GLPlayImg : VideoUGen {
    *fr { arg imgID;
        ^this.new(\control, imgID)
    }
}

GLPrevFrame : VideoUGen {
    *fr {
        ^this.new(\control)
    }
}

GLPrevFrame2 : VideoUGen {
    *fr { arg coords;
        ^this.new(\control, coords)
    }
}

GLRGB : VideoUGen {
    *fr { arg in, r, g, b;
        ^this.new(\control, in, r, g, b)
    }
}

GLRGBA : VideoUGen {
    *fr { arg in, r, g, b, a;
        ^this.new(\control, in, r, g, b, a)
    }
}

GLAdd : VideoUGen {
    *fr { arg in1, in2;
        ^this.new(\control, in1, in2)
    }
}

GLMix : VideoUGen {
    *fr { arg in1, in2, mix;
        ^this.new(\control, in1, in2, mix)
    }
}

GLBlend : VideoUGen {
    *fr { arg backdrop, source, blendMode=0, mixVal=1;
        ^this.new(\control, backdrop, source, blendMode, mixVal)
    }
}



FlipX : VideoUGen {
    *fr { arg in;
        ^this.new(\control, in)
    }
}

FlipY : VideoUGen {
    *fr { arg in;
        ^this.new(\control, in)
    }
}

MirrorX : VideoUGen {
    *fr { arg in, mirrorXPos, polarity=0;
        ^this.new(\control, in, mirrorXPos, polarity)
    }
}

MirrorY : VideoUGen {
    *fr { arg in, mirrorYPos, polarity=0;
        ^this.new(\control, in, mirrorYPos, polarity)
    }
}

Ripple : SHKUGen {
    *fr { arg in, time, speed, strength, frequency;
        ^this.new(\control, in, time, speed, strength, frequency)
    }
}

Rotate : VideoUGen {
    *fr { arg in, angle;
        ^this.new(\control, in, angle)
    }
}

Scale2 : VideoUGen {
    *fr { arg in, scale=1;
        ^this.new(\control, in, scale)
    }
}

ScaleXY : VideoUGen {
    *fr { arg in, scaleX=1, scaleY=1;
        ^this.new(\control, in, scaleX, scaleY)
    }
}

Translate : VideoUGen {
    *fr { arg in, translateX=0, translateY=0;
        ^this.new(\control, in, translateX, translateY)
    }
}



PlayVid : VideoUGen {
    *fr { arg vidID, rate=1.0, loop=1.0;
        ^this.new(\control, vidID, rate, loop)
    }
}

VidRd : VideoUGen {
    *fr { arg vidID, phase=0.0;
        ^this.new(\control, vidID, phase)
    }
}

DelBufRd : VideoUGen {
    *fr { arg bufID;
        ^this.new(\control, bufID)
    }
}

DelBufWr : VideoUGen {
    *fr { arg in, bufID;
        ^this.new(\control, in, bufID)
    }
}



GLFunc1 : VideoUGen {
    *fr { arg arg1;
        ^this.new(\control, arg1)
    }
}

GLFunc2 : VideoUGen {
    *fr { arg arg1, arg2;
        ^this.new(\control, arg1, arg2)
    }
}

GLFunc3 : VideoUGen {
    *fr { arg arg1, arg2, arg3;
        ^this.new(\control, arg1, arg2, arg3)
    }
}

GLFunc4 : VideoUGen {
    *fr { arg arg1, arg2, arg3, arg4;
        ^this.new(\control, arg1, arg2, arg3, arg4)
    }
}

GLFunc5 : VideoUGen {
    *fr { arg arg1, arg2, arg3, arg4, arg5;
        ^this.new(\control, arg1, arg2, arg3, arg4, arg5)
    }
}

GLFunc6 : VideoUGen {
    *fr { arg arg1, arg2, arg3, arg4, arg5, arg6;
        ^this.new(\control, arg1, arg2, arg3, arg4, arg5, arg6)
    }
}

GLFunc7 : VideoUGen {
    *fr { arg arg1, arg2, arg3, arg4, arg5, arg6, arg7;
        ^this.new(\control, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
    }
}

GLFunc8 : VideoUGen {
    *fr { arg arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8;
        ^this.new(\control, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
    }
}

GLFunc9 : VideoUGen {
    *fr { arg arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9;
        ^this.new(\control, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
    }
}

GLFunc10 : VideoUGen {
    *fr { arg arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10;
        ^this.new(\control, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10)
    }
}
