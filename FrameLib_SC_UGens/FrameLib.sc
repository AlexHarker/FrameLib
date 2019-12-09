
FLParam : UGen {

	*ir { arg a;

		var args = ['init'];
		var i = 0;

		while ( {(i + 1) < a.size} , {
			var tag = a[i];
			var b = [a[i + 1]];
			i = i + 2;
			while( { (i < a.size) && (a[i].isKindOf(Symbol) == false) }, {
				b = b.add(a[i]);
				i = i + 1;
			});

			args = args ++ this.parseTag(tag, b);
		});

		^this.new1( *args );
	}

	*parseTag { arg tag, val;

		var detectSymbol = (val.size == 1) && val[0].isKindOf(Symbol);
		var args1 = this.parseSymbol(tag);
		var args2 = val;

		switch (detectSymbol,
			true, { args2 = this.parseSymbol(val[0]); },
			false, { args2 = val; });

		^[detectSymbol.if(1, 0), args1.size].addAll(args1).add(args2.size).addAll(args2);
	}

	*parseSymbol { arg str;

		var ascii = str.ascii;
		var args = Array.fill((ascii.size / 3).asInteger, { arg i;
			ascii[i * 3] + (ascii[i * 3 + 1] * 256) + (ascii[i * 3 + 2] * 65536) });

		switch (ascii.size % 3,
			0, { ^args; },
			1, { ^args.add(ascii[args.size * 3]); },
			2, { ^args.add(ascii[args.size * 3] + (ascii[args.size * 3 + 1] * 256)); });
	}
}

FLObject : UGen {
}

FLInterval : FLObject {
	*fr { arg initParams = '', params = 0.0;
        ^this.multiNew('audio', FLParam.ir(initParams), params)
    }
}

FLRead : FLObject {
	*fr { arg in1 = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, params)
    }
}

FLMap : FLObject {
	*fr { arg in1 = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, params)
    }
}

FLWindow : FLObject {
	*fr { arg in = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in, params)
    }
}

// IO

FLSource : FLObject {
	*fr { arg in1 = 0.0, trigger = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('audio', FLParam.ir(initParams), in1, trigger, params)
    }
}

FLSink : FLObject {
   *fr { arg in = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('audio', FLParam.ir(initParams), in, params)
    }
}

FLTrace : FLObject {
   *fr { arg in = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('audio', FLParam.ir(initParams), in, params)
    }
}

// Generators

FLRamp : FLObject {
	*fr { arg in1 = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, params)
    }
}

FLRand : FLObject {
	*fr { arg in1 = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, params)
    }
}

FLUniform : FLObject {
	*fr { arg in1 = 0.0, params = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, params)
    }
}

// Unary

FLNot : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLAcos : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLAsin : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLAtan : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLAcosh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLAsinh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLAtanh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLCos : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLSin : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLTan : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}
FLCosh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLSinh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLTanh : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLLog : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLLog2 : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLLog10 : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLExp : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLExp2 : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLAbs : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLCeil : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLFloor : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLRound : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLTrunc : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLSqrt : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLCbrt : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLErf : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

FLErfc : FLObject {
	*fr { arg in = 0.0;
		^this.multiNew('control', FLParam.ir(), in)}
}

// Binary

FLMul : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}
FLAdd : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}
FLSub : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}
FLDiv : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}

FLPow : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}
FLAtan2 : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}
FLHypot : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}
FLCopySign : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}
FLMin : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
        ^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}
FLMax : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
		^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}
FLModulo : FLObject {
	*fr { arg in1 = 0.0, in2 = 0.0, initParams = "";
		^this.multiNew('control', FLParam.ir(initParams), in1, in2)}
}
