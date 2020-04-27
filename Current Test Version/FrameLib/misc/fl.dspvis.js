mgraphics.init();

inlets  = 1;
outlets = 0;
setinletassist(0, 'Ctrl In');

boxSize    = get_box_size();
// [backCol, borderCol, borderWid, 
//  xAxisCol, xAxisCol, xAxisXOffset, 
//  sinYOffset, sineCol, sineWidth, 
//  yCol, yWidth]:
style      = [
				[0.200, 0.200, 0.200, 1.000], 
				[0.502, 0.502, 0.502, 1.000], 
				4, 
                [0.502, 0.502, 0.502, 1.000], 
				2, 
				5, 
                5, 
				[0.929, 0.929, 0.353, 1.000], 
				4, 
                [0.969, 0.969, 0.969, 0.500], 
				2
			]
res        = 512;
maxSineX   = 6.285;
sinePoints = get_sine_points();
// 0 = points, 1 = lines, 2 = both:
drawMode   = 0;
drawY      = true;

function paint(){
    draw_background();


    draw_y();

    draw_sine();

    draw_x_axis();

    draw_border();
	
	set_x_range(6.285);
}

function reset_params(){
    res      = 512;
    drawMode = 0;
    maxSineX = 6.285;
    drawY    = true; 
    sinePoints = get_sine_points();
    mgraphics.redraw();
}

function set_x_range(x){
    maxSineX = x;
    sinePoints = get_sine_points();
    mgraphics.redraw();
}

function set_draw_y(x){
    if(x == 0){
        drawY = false;
    }
    else{
        drawY = true;
    }

    mgraphics.redraw();
}

function set_draw_mode(x){
    drawMode = x;
    mgraphics.redraw();
}

function set_res(x){
    res = x;
    sinePoints = get_sine_points();
    mgraphics.redraw();
}

function get_sine_points(){
    points      = [];
    incr        = maxSineX / res;
    screenMin   = boxSize[1] - (style[2] + style[6]);
    screenMax   = style[2] + style[6];
    screenRange = screenMax - screenMin;
    xIncr       = (boxSize[0] - (style[5] * 2)) / res;

    for(i = 0; i < res + 1; i++){
        pointArray = [];
        point      = Math.sin(i * incr);
        screenY    = ((screenRange * (point + 1)) * 0.5) + screenMin;
        screenX    = style[5] + (i * xIncr);

        pointArray.push(screenX);
        pointArray.push(screenY);
        
        points.push(pointArray)
    }

    return points
}

function draw_y(){
    with(mgraphics){
        set_source_rgba(style[9]);
        set_line_width(style[10]);
        for(i = 0; i < sinePoints.length; i++){
            move_to(sinePoints[i][0], boxSize[1] * 0.5);
            line_to(sinePoints[i][0], sinePoints[i][1]);
        }
        stroke();
    }
}

function draw_sine(){
    with(mgraphics){
        if(drawMode == 0){
            set_source_rgba(style[7]);
            for(i = 0; i < sinePoints.length; i++){
                ellipse(sinePoints[i][0] - (style[8] * 0.5), sinePoints[i][1] - (style[8] * 0.5), style[8], style[8]);
            }
            fill();
        }
        else if(drawMode == 1){
            set_source_rgba(style[7]);
            set_line_width(style[8]);
            for(i = 0; i < sinePoints.length - 1; i++){
                move_to(sinePoints[i][0], sinePoints[i][1]);
                line_to(sinePoints[i + 1][0], sinePoints[i + 1][1])
            }
            stroke();
        }
        else if(drawMode == 2){
            set_source_rgba(style[9]);
            set_line_width(style[10]);
            for(i = 0; i < sinePoints.length - 1; i++){
                move_to(sinePoints[i][0], sinePoints[i][1]);
                line_to(sinePoints[i + 1][0], sinePoints[i + 1][1])
            }
            stroke();
            
            set_source_rgba(style[7]);
            for(i = 0; i < sinePoints.length; i++){
                ellipse(sinePoints[i][0] - (style[8] * 0.5), sinePoints[i][1] - (style[8] * 0.5), style[8], style[8]);
            }
            fill();
        }
    }
}

function draw_background(){
    with(mgraphics){
        set_source_rgba(style[0]);
        rectangle(0, 0, boxSize[0], boxSize[1]);
        fill();
    }
}

function draw_border(){
    with(mgraphics){
        set_source_rgba(style[1]);
        rectangle(0, 0, boxSize[0], style[2]);
        rectangle(0, boxSize[1] - style[2], boxSize[0], style[2]);
        fill();
    }
}

function draw_x_axis(){
    with(mgraphics){
        set_source_rgba(style[3])
        rectangle(style[5], (boxSize[1] * 0.5) - (style[4] * 0.5), boxSize[0] - (style[5] * 2), style[4]);
        fill();
    }
}

function get_box_size(){
    xSize     = this.box.rect[2] - this.box.rect[0];
    ySize     = this.box.rect[3] - this.box.rect[1];
    sizeArray = [xSize, ySize];

    return sizeArray;
}

function onresize(){
    boxSize = get_box_size();
    sinePoints = get_sine_points();
    mgraphics.redraw();
}