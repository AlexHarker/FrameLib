mgraphics.init();

var autowatch = 1;
var mouseState = [0,0,0,0,0,0,0,0]; // x, y, click, dblclick, dragOn, dragOff, button, shift
var clickEnable = [false, "none"];
var dragEnable = [false, "none"];
var btnCol = [0.647, 0.647, 0.647, 1.000];
var btnColSel = [1.000, 0.710, 0.196, 1.000];
var txtCol = [0.000, 0.000, 0.000, 1.000];
var selectBoxCol = [0.996, 0.431, 0.431, 0.390];
var selCol = [0.996, 0.431, 0.431, 1.000];
var hoverCol = [0.498, 0.498, 0.498, 1.000];
var backCol = [0.200, 0.200, 0.200, 1.000];
var btnBorder = 1;
var fontFace = "Lato";
var btnFontSize = 13;
var titleFontSize = 16;
var currentInfoText = "FrameLib Tutorials";
var cameraFocus = 0;
var sectionOneOpen = false;
var sectionTwoOpen = false;
var sectionThreeOpen = false;
var numBoxes = 3;
var yOffset = 0;
var additionalYoffset = 0;
var additionalYoffset2 = 0;
var additionalYoffset3 = 0;
var currentScroll = 0;
var slideWid = 10;
var maxScroll = 0;
var scrollCLickable = false;
var p = this.patcher;
var patcher_path = p.filepath 
var txt_wrap_boundary = 75;

function wordwrap(str, width, brk, cut) 
{
    brk = brk || '\\cr';
    cut = cut || false;
    if (!str) { return str; }
    var regex = '.{1,' +width+ '}(\\s|$)' + (cut ? '|.{' +width+ '}|.+$' : '|\\S+?(\\s|$)');
    var v=str.match( RegExp(regex, 'g') );
    return v;
}

// Use the tlookup to populate this stuff
var tlookup = new Dict()
tlookup.import_json("/Users/jamesbradbury/FrameLib/Current Test Version/FrameLib/interfaces/FrameLib-obj-tlookup.json")
var difficulties = tlookup.getkeys();

// Transfer the coll data into a flat structure (much easier to parse later)
// var tutorial_info = new Dict()
tutorial_title = []
tutorial_digest = []
for (i = 0; i < difficulties.length; i++) {
    difficulty = difficulties[i]
    temp_dict = tlookup.get(difficulty)
    entries = temp_dict.getkeys()
    for (j=0; j < entries.length; j++) {
        digest = temp_dict.get(entries[j])
        title = entries[j]
        tutorial_title.push(title)
        tutorial_digest.push(digest)
    }
}

num_tutorials = tutorial_title.length

function paint() {

    windWid = this.box.rect[2] - this.box.rect[0];
    windHei = this.box.rect[3] - this.box.rect[1];
    background(0, 0, windWid, windHei);

    for (i=0; i<num_tutorials; i++) {
        txtBtn(0, (i+1)*35 + 20 , windWid, 35, tutorial_title[i], "open_a_patcher()", tutorial_digest[i], title + "btn", 0);
    }

    // vSlider(windWid - 20, 20, 20, windHei - 40, "currentScroll", 0, maxScroll, "scrollSlid", "test_slider_func();", "Scroll the menu here", 0);

    txtBoxFill(0, 0, windWid, 40, "FrameLib Tutorials", "FrameLib Tutorials", 0, "cen");

    infoBar(0, windHei-50, windWid, 60, 0);
}

function open_a_patcher()
{
    // Connected to a pcontrol thing so just send the filname out of the 0th outlet

    homeDirec = p.filepath; // This is the current patcher's filepath so just chop off the end and work from there.
    patcherName = p.name;
    post(homeDirec + "\n")

    aPossiblePatcher = homeDirec.replace(patcherName + ".maxpat") + "lalala.maxpat";

    outlet(0, "load " + aPossiblePatcher);
}

function update_scroll_size()
{
    if(numBoxes * 20 > windHei - 40)
    {
        mustScroll = true;
    }
    else
    {
        mustScroll = false;
    }

    if(mustScroll)
    {
        maxScroll = 1;
        scrollCLickable = true;
        slideWid = ((this.box.rect[3] - this.box.rect[1]) - 40) * 0.2;
    }
    else
    {
        scrollCLickable = false;
        currentScroll = 0.5;
        slideWid = windHei - 40;
        maxScroll = 1;
        yOffset = 0;
    }
}

function test_slider_func()
{
    gap = ((this.box.rect[3] - this.box.rect[1]) - 40) - (numBoxes * 20);
    yOffset = currentScroll * gap;
}

function background(x, y, w, h)
{
    with (mgraphics)
    {
        set_source_rgba(backCol);
        rectangle(x, y, w, h);
        fill();

        set_source_rgba(txtCol);
        set_line_width(btnBorder);
        move_to(x + 1, y + 1);
        line_to(x + w, y + 1);
        line_to(x + w, y + h);
        line_to(x + 1, y + h);
        line_to(x + 1, y - (btnBorder / 2));
        stroke();
    }
}

function txtBtn(x, y, w, h, txt, func, desc, btnName, camLevel)
{
    // ##############################
    // Create a button with a text label that is tied to a function.
    // ##############################

    with (mgraphics)
    {
        unselectedCol = btnCol;

        if(cameraFocus == camLevel)
        {
            if (mouseState[0] > x && mouseState[0] < x + w && mouseState[1] > y && mouseState[1] < y + h)
            {
                currentInfoText = desc;
                unselectedCol = hoverCol;
            }
        }

        var trimLen = true;
        while(trimLen)
        {
            txt_len = text_measure(txt);
            if(txt_len[0] > w)
            {
                if (txt.slice(-5) == "(...)")
                {
                    txt = txt.slice(0, -5)
                }
                txt = txt.slice(0, -1);
                txt = txt + "(...)";
            }
            else
            {
                trimLen = false;
            }
        }

        var selected = false;

        if(cameraFocus == camLevel)
        {
            if (mouseState[0] > x && mouseState[0] < x + w && mouseState[1] > y && mouseState[1] < y + h && mouseState[2] == 1)
            {
                selected = true;
                clickEnable[0] = true;
                clickEnable[1] = btnName;
            }
        }

        if(cameraFocus == camLevel)
        {
            if(clickEnable[0])
                if(clickEnable[1] == btnName)
                    if(mouseState[2] == 0)
                    {
                        eval(func);
                        clickEnable[0] = false;
                        clickEnable[1] = "none";
                    }
        }


        if(selected)
            {
                set_source_rgba(btnColSel);
            }

        else
            set_source_rgba(unselectedCol);

        rectangle(x, y, w, h);
        fill();

        set_source_rgba(txtCol);
        set_line_width(btnBorder);
        move_to(x, y);
        line_to(x + w, y);
        line_to(x + w, y + h);
        line_to(x, y + h);
        line_to(x, y - (btnBorder / 2));
        stroke();

        set_source_rgba(txtCol);
        select_font_face(fontFace);
        set_font_size(btnFontSize);
        txt_len = text_measure(txt);
        move_to(x + 5, (y + (h / 2)) + ((txt_len[1] / 2) / 2));
        text_path(txt);
        fill();
    }
}

function infoBar(x, y, w, h, camLevel) {
    mgraphics.select_font_face(fontFace);
    mgraphics.set_font_size(btnFontSize);

    with (mgraphics)
    {
        if(cameraFocus == camLevel)
        {
            if (mouseState[0] > x && mouseState[0] < x + w && mouseState[1] > y && mouseState[1] < y + h)
                currentInfoText = "Info bar.";
        }

        txt = currentInfoText;

        information_text = wordwrap(txt, txt_wrap_boundary);

        set_source_rgba(btnCol);
        rectangle(x, y, w, h);
        fill();
        
        set_source_rgba(txtCol);
        set_line_width(btnBorder);
        move_to(x, y);
        line_to(x + w, y);
        line_to(x + w, y + h);
        line_to(x, y + h);
        line_to(x, y - (btnBorder / 2));
        stroke();

        

        for (i=0; i<information_text.length; i++) {
            // move_to(x + 5, ((i*20) + y + (h / 2)) + ((txt_len[1] / 2) / 2));
            horiz_text = x+5
            verti_text = (i*20) + (y + 20)
            move_to(horiz_text, verti_text);
            mgraphics.set_source_rgba(txtCol);
            text_path(information_text[i]);
        }
        fill();
    }
}

function txtBoxFill(x, y, w, h, txt, desc, camLevel, align)
{
    // ##############################
    // Draw some text with chosen alignment (cen, left, right).
    // ##############################

    with (mgraphics)
    {
        set_source_rgba(btnCol);
        rectangle(x, y, w, h);
        fill();

        set_source_rgba(txtCol);
        set_line_width(btnBorder);
        move_to(x, y);
        line_to(x + w, y);
        line_to(x + w, y + h);
        line_to(x, y + h);
        line_to(x, y - (btnBorder / 2));
        stroke();

        if(cameraFocus == camLevel)
        {
            if(mouseState[0] > x && mouseState[0] < x + w && mouseState[1] > y && mouseState[1] < y + h)
            {
                currentInfoText = desc;
            }
        }

        var trimLen = true;
        while(trimLen)
        {
            txt_len = text_measure(txt);
            if(txt_len[0] > w - 5)
            {
                if (txt.slice(-5) == "(...)")
                {
                    txt = txt.slice(0, -5)
                }
                txt = txt.slice(0, -1);
                txt = txt + "(...)";
            }
            else
            {
                trimLen = false;
            }
        }

        set_source_rgba(txtCol);
        select_font_face(fontFace);
        set_font_size(titleFontSize);

        if(align == "cen")
        {
            move_to((x + (w / 2)) - (txt_len[0] / 2), (y + (h / 2)) + ((txt_len[1] / 2) / 2));
        }
        if(align == "left")
        {
            move_to(x + 5, (y + (h / 2)) + ((txt_len[1] / 2) / 2));
        }
        if(align == "right")
        {
            move_to(((x + w) - txt_len[0]) - 5, (y + (h / 2)) + ((txt_len[1] / 2) / 2));
        }

        text_path(txt);
        fill();
    }
}

function vSlider(x, y, w, h, variable, min, max, btnName, func, desc, camLevel)
{
    // ##############################
    // Creates a horizontal slider tied to a variable and a function.
    // ##############################

    with (mgraphics)
    {
        unselectedCol = btnCol;

        if(scrollCLickable)
        {
            if(cameraFocus == camLevel)
            {
                if (mouseState[0] > x && mouseState[0] < x + w && mouseState[1] > y && mouseState[1] < y + h)
                {
                    currentInfoText = desc;
                    unselectedCol = hoverCol
                }
            }
        }


        if (max > min)
            var inverted = false;
        else
            var inverted = true;

        if(inverted)
        {
            var range = min - max;
            var offset = max;
        }
        else
        {
            var range = max - min;
            var offset = min;
        }
        var theVar = eval(variable)

        if (inverted)
            var scaledVar = ((theVar * -1)  - offset) / range;
        else
            var scaledVar = (theVar - offset) / range;

        var slideCol = unselectedCol;
        var currentPos = (((scaledVar * h) / 1) + (y + btnBorder));

        set_source_rgba(btnCol);
        rectangle(x, y, w + slideWid + btnBorder, h);
        fill();
        set_source_rgba(txtCol);
        set_line_width(btnBorder);
        move_to(x, y);
        line_to(x + w, y);
        line_to(x + w, y + h);
        line_to(x, y + h);

        line_to(x, y - (btnBorder / 2));
        stroke();

        move_to(x + (w / 2), y + btnBorder);
        line_to(x + (w / 2), y + h - btnBorder);
        stroke();

        if(scrollCLickable)
        {
            if(cameraFocus == camLevel)
            {
                if (mouseState[0] > currentPos - (slideWid / 2) && mouseState[0] <= currentPos + (slideWid / 2) && mouseState[1] > y && mouseState[1] < y + h && mouseState[2] == 1)
                {
                    dragEnable[0] = true;
                    dragEnable[1] = btnName;
                }
                else if (mouseState[0] > x && mouseState[0] < x + w && mouseState[1] > y && mouseState[1] < y + h && mouseState[2] == 1)
                {
                    dragEnable[0] = true;
                    dragEnable[1] = btnName;

                    var newVal = (mouseState[1] - y) / h;
                    if(newVal > 1)
                        newVal = 1;
                    if (newVal < 0)
                        newVal = 0;

                    if (inverted)
                        var newValScaled = ((newVal * range) + offset) * -1;
                    else
                        var newValScaled = (newVal * range) + offset;
                    eval(variable + "=" + newValScaled);

                    eval(func);

                    var slideCol = btnColSel;
                }
            }
        }

        if(scrollCLickable)
        {
            if(cameraFocus == camLevel)
            {
                if (dragEnable[0])
                {
                    if(dragEnable[1] == btnName)
                    {
                        var newVal = (mouseState[1] - y) / h;
                        if(newVal > 1)
                            newVal = 1;
                        if (newVal < 0)
                            newVal = 0;

                        if (inverted)
                            var newValScaled = ((newVal * range) + offset) * -1;
                        else
                            var newValScaled = (newVal * range) + offset;
                        eval(variable + "=" + newValScaled);

                        eval(func);

                        var slideCol = btnColSel;

                        if(mouseState[6] == 0)
                        {
                            var slideCol = btnCol;
                            dragEnable[0] = false;
                            dragEnable[1] = "none";
                        }
                    }
                }
            }
        }


        set_source_rgba(slideCol);
        slideY = currentPos - (slideWid / 2);
        rectangle(x, slideY, w, slideWid);
        fill();

        set_source_rgba(txtCol);
        set_line_width(btnBorder);
        move_to(x, slideY);
        line_to(x + w, slideY);
        line_to(x + w, slideY + slideWid);
        line_to(x, slideY + slideWid);
        line_to(x, slideY);
        stroke();
    }
}

function onidle (x, y, button, shift)
{
    mouseState[0] = x;
    mouseState[1] = y;
    mouseState[2] = 0;
    mouseState[3] = 0;
    mouseState[6] = button;
    mouseState[7] = shift;
    mgraphics.redraw();
}
function onclick(x, y, button, shift)
{
    mouseState[0] = x;
    mouseState[1] = y;
    mouseState[2] = 1;
    mouseState[3] = 0;
    mouseState[6] = button;
    mouseState[7] = shift;
    mgraphics.redraw();
}
function ondblclick(x, y, button, shift)
{
    mouseState[0] = x;
    mouseState[1] = y;
    mouseState[2] = 0;
    mouseState[3] = 1;
    mouseState[6] = button;
    mouseState[7] = shift;
    mgraphics.redraw();
}
function ondrag(x, y, button, shift)
{
    mouseState[0] = x;
    mouseState[1] = y;
    mouseState[2] = 0;
    mouseState[3] = 0;
    if (button == 0)
    {
        mouseState[4] = 0;
        mouseState[5] = 1;
    }
    if (button == 1)
    {
        mouseState[4] = 1;
        mouseState[5] = 0;
    }
    mouseState[6] = button;
    mouseState[7] = shift;
    mgraphics.redraw();
}