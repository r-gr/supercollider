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

GLPlayVid : VideoUGen {
    *fr { arg vidID;
        ^this.new(\control, vidID)
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

GLShowImgTex : VideoUGen {
    *fr { arg imgID, coords;
        ^this.new(\control, imgID, coords)
    }
}

GLShowVidTex : VideoUGen {
    *fr { arg vidID, coords;
        ^this.new(\control, vidID, coords)
    }
}

GLTexCoords : VideoUGen {
    *fr {
        ^this.new(\control)
    }
}

GLTexFlipX : VideoUGen {
    *fr { arg in;
        ^this.new(\control, in)
    }
}

GLTexFlipY : VideoUGen {
    *fr { arg in;
        ^this.new(\control, in)
    }
}

GLTexMirrorX : VideoUGen {
    *fr { arg in, mirrorXPos, polarity=0;
        ^this.new(\control, in, mirrorXPos, polarity)
    }
}

GLTexMirrorY : VideoUGen {
    *fr { arg in, mirrorYPos, polarity=0;
        ^this.new(\control, in, mirrorYPos, polarity)
    }
}

GLTexScale : VideoUGen {
    *fr { arg in, scaleX=1, scaleY=1;
        ^this.new(\control, in, scaleX, scaleY)
    }
}

GLTexTrans : VideoUGen {
    *fr { arg in, translateX=0, translateY=0;
        ^this.new(\control, in, translateX, translateY)
    }
}

GLRotate : VideoUGen {
    *fr { arg coords, angle;
        ^this.new(\control, coords, angle)
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
