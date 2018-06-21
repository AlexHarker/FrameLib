/**
	@file
	fl.plus~.c
	
	@name
	fl.plus~
	
	@realname
	fl.plus~
 
	@type
	object
	
	@module
	FrameLib
 
	@author
	Alex Harker
	
	@digest
	Adds values in two input frames.
	
	@description
    <o>fl.plus~</o> calculates pairs of values in turn. The result is an output frame at least as long as the smaller of the two inputs. 
    When frames mismatch in size the result depends on the setting of the mismatch parameter. 
    Either or both inputs may be set to trigger output.

	@discussion

	
	@category
	FrameLib, Binary Ops
 
	@keywords
	addition, add, plus, +~, +
 
	@seealso
    fl.minus~, fl.times~, fl.divide~, fl.pow~
	
	@owner
	Alex Harker

	@in 0 @type symbol @digest Left Operand
	@in 1 @type symbol @digest Right Operand
	@out 1 @type symbol @digest Result
	@arg 0 @name mismatch @optional 0 @type int @digest Sets the mismatch mode (wrap, shrink, pad_in, pad_out)
	@arg 1 @name trigger_ins @optional 0 @type int @digest Sets which inputs trigger outputs
	@arg 2 @name pad @optional 0 @type int @digest Sets the value used for padding (for pad_in or pad_out modes)
 */