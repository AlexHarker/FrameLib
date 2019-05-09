autowatch = 1;
inlets=1;
outlets=1;

this.box.message("border", 0);
this.box.message("ignoreclick", 1);
mgraphics.init();
mgraphics.relative_coords = 0;
mgraphics.autofill = 0;
var alpha = 1.0;
var obj = jsarguments[1];
var dict = new Dict();
var inner;
var innerkeys;
var params;

function init()
{
	dict.import_json("FrameLib-obj-jlookup.json");
	if (dict.contains(obj)) {
		post("yes");
	};
		
}
init();

function paint()
{
	if(name){
    	with(mgraphics) 
    	{
			var bgcolor = this.patcher.getattr("locked_bgcolor");
			set_source_rgba(bgcolor);
			paint();
    		move_to(4, 40);
    		select_font_face("Lato");
 			var textcolor = this.patcher.getattr("textcolor");
			set_source_rgba(textcolor);
       		set_font_size(48);
        	show_text(name);
			move_to(4, 70);
			set_font_size(13);
			if (shortDesc)
				show_text(shortDesc);
			move_to(4, 90);
			var detailstextcolor = this.patcher.getattr("accentcolor");
			set_source_rgba(detailstextcolor);
			fill();
    	}
	}
}
