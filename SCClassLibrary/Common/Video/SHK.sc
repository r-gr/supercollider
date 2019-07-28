SHKUGen : UGen {
    *new { arg ... args;
        ^this.new1( *args );
    }
}

SHKCheckerboard : SHKUGen {
    *fr { arg in, rows, cols;
        ^this.new(\control, in, rows, cols)
    }
}

SHKCircleWave : SHKUGen {
    *fr { arg texture, time, speed, brightness, strength, density, center;
        ^this.new(\control, texture, time, speed, brightness, strength, density, center)
    }
}

SHKColorInvert : SHKUGen {
    *fr { arg in;
        ^this.new(\control, in)
    }
}

SHKDesaturate : SHKUGen {
    *fr { arg in, strength;
        ^this.new(\control, in, strength)
    }
}
