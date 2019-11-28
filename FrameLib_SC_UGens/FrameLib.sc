
FLParam : UGen {

	*ir { arg tag, a;

		^this.parseTag(tag, a);
	}

	*parseTag { arg tag, val;

		if (tag.isKindOf(Symbol))
		{
			var args1 = this.parseSymbol(tag);
			var args2 = this.parseValue(val);

			var newArgs = ['init', val.isKindOf(Symbol).if(1, 0), args1.size].addAll(args1).add(args2.size).addAll(args2);

			^this.new1( *newArgs );
		}
	}

	*parseSymbol { arg str;

		var ascii = str.ascii;
		var loopmod = ascii.size % 3;
		var args;

		args = Array.fill((ascii.size / 3).asInteger, { arg i; ascii[i * 3] + (ascii[i * 3 + 1] * 256) + (ascii[i * 3 + 2] * 65536) });

		switch (loopmod,
			0, { ^args; },
			1, { ^args.add(ascii[args.size * 3]); },
			2, { ^args.add(ascii[args.size * 3] + (ascii[args.size * 3 + 1] * 256)); });
	}

	*parseValue { arg a;

		switch(a.isKindOf(Symbol).asInt + (a.isKindOf(Array).asInt * 2),
			0, { ^[a]; },
			1, { ^this.parseSymbol(a); },
			2, { ^a; });
	}
}

FLObject : UGen {
	classvar paramCount;

	*initClass {
		paramCount = 0;
	}

	*makeNewParams { arg paramString = '', inputString = " ";
		var count = -1;
		if (paramString.isKindOf(String), { if(paramString.size() > 0, {
			count = paramCount;
			Server.local.sendMsg("/cmd", "FLParameters", paramCount, paramString, inputString);
			paramCount = paramCount + 1;
		})})
		^count;
	}
}

FLInterval : FLObject {
	*fr { arg paramString = '', params = 0.0;
        ^this.multiNew('audio', this.makeNewParams(paramString), params)
    }
}

FLRead : FLObject {
	*fr { arg in1 = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', this.makeNewParams(initParams), in1, params)
    }
}

FLMap : FLObject {
	*fr { arg in1 = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', this.makeNewParams(initParams), in1, params)
    }
}

FLWindow : FLObject {
	*fr { arg in = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', this.makeNewParams(initParams), in, params)
    }
}

// IO

FLSource : FLObject {
	*fr { arg in1 = 0.0, trigger = 0.0, params = 0.0;
        ^this.multiNew('audio', this.makeNewParams(""), in1, trigger, params)
    }
}

FLSink : FLObject {
   *fr { arg in = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('audio', this.makeNewParams(initParams), in, params)
    }
}

FLTrace : FLObject {
   *fr { arg in = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('audio', this.makeNewParams(initParams), in, params)
    }
}

// Generators

FLRamp : FLObject {
	*fr { arg in1 = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', this.makeNewParams(initParams), in1, params)
    }
}

FLRand : FLObject {
	*fr { arg in1 = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', this.makeNewParams(initParams), in1, params)
    }
}

FLUniform : FLObject {
	*fr { arg in1 = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', this.makeNewParams(initParams), in1, params)
    }
}

// Unary

FLNot : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLAcos : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLAsin : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLAtan : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLAcosh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLAsinh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLAtanh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLCos : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLSin : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLTan : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}
FLCosh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLSinh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLTanh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLLog : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLLog2 : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLLog10 : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLExp : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLExp2 : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLAbs : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLCeil : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLFloor : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLRound : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLTrunc : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLSqrt : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLCbrt : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLErf : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}

FLErfc : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', -1, in)}
}


// Binary

FLMul : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
        ^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}
FLAdd : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
        ^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}
FLSub : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
        ^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}
FLDiv : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
        ^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}

FLPow : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
        ^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}
FLAtan2 : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
        ^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}
FLHypot : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
        ^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}
FLCopySign : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
        ^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}
FLMin : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
        ^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}
FLMax : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
		^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}
FLModulo : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, params = "";
		^this.multiNew('control', this.makeNewParams(params), in1, in2)}
}
