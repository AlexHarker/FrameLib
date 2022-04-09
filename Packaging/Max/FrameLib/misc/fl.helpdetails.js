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
var shortDesc = "";
var longDesc = "";

function init()
{
    dict = max.getrefdict(name);
    if (typeof(dict) == "object") {
        shortDesc = dict.get("digest");
        longDesc = dict.get("description");
		longDesc = longDesc.replace(/More info.+Tutorial.+\./, " ");
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
            if(longDesc!=null)
            {
                wordwrap(longDesc, 105);
            }
            fill();
        }
    }
}

function wordwrap(str, width, brk, cut) 
{
    brk = brk || '\\cr';
    width = jsarguments[2] || width;
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