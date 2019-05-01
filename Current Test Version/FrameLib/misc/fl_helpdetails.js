autowatch = 1;
inlets=1;
outlets=1;

this.box.message("border", 0);
this.box.message("ignoreclick", 1);
mgraphics.init();
mgraphics.relative_coords = 0;
mgraphics.autofill = 0;
var alpha = 1.0;
var name = jsarguments[1];
var dict;
var paramDesc = "";

function init()
{
	dict = max.getrefdict(name);
	if (typeof(dict) == "object") {
		paramDesc = dict.get("description");
		keys = dict.getkeys();
		post(keys);
		dict.freepeer();

	}
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
    		select_font_face("Lato");
 			var textcolor = this.patcher.getattr("textcolor");
			set_source_rgba(textcolor);
       		set_font_size(13);
			move_to(4, 100);
        	show_text(name);
			move_to(4, 200);
            // Parameters
            show_text(paramDesc);
			fill();
    	}
	}
}

function wordwrap(str, width, brk, cut) 
{
 	if(jsarguments[2]==null){jsarguments[2]=95};
    brk = brk || '\\cr';
    width = width || jsarguments[2];
    cut = cut || false;
    if (!str) { return str; }
    var regex = '.{1,' +width+ '}(\\s|$)' + (cut ? '|.{' +width+ '}|.+$' : '|\\S+?(\\s|$)');
    var v=str.match( RegExp(regex, 'g') );
	for(i=0;i<=v.length;i++)
		{	
			mgraphics.show_text(v[i], 1);
			mgraphics.move_to(4, 105+15*i);
		}
	return;
}
