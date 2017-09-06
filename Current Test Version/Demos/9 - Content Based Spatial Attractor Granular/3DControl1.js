
outlets = 2;

mgraphics.init();
mgraphics.relative_coords = 1;
mgraphics.autofill = 0;

var area_fill = [0.4, 0.4, 0.4, 1];
var cross_stroke = [0.6, 0.6, 0.6, 1];
var handle_fill1 = [0.884, 0.285, 0.35, 1.];
var active_fill = [1, 1, 1, 1];
var selected_fill = [0.2, 1, 0.2, 1];
var origin_stroke = [0, 0, 0, 1];
		
var num_points = 0;
var max_points = 10;

var handleX = [];
var handleY = [];
var handleZ = [];

var azimuth = 0;
var elevation = 0;

var edit_vector = [0, 1];

var area_r = 0.9;
var handle_r = 0.035;
var origin_or = 0.02;
var origin_ir = 0.01;

var handle_spacing = 3.0;
var handle_select = 0;
var point_select = -1;

var space = (1 + handle_spacing) * handle_r;

var last_x;
var last_y;

var flip = 0;
var last_shift = 0;;

var mode = 0;

var elevationMax = Math.PI / 2;

var z_scale = 3.0;
var y_scale = 0.75 * (z_scale / 2);

mgraphics.redraw();

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

function set(idx, x, y, z)
{
	if (idx < 0 || idx >= num_points || !num_points)
		return;
	
	z = clip(z, 0, 1);
	
	var radius = Math.sqrt(x * x + y * y + z * z);
	
	if (radius > 1.0)
	{
		x /= radius;
		y /= radius;
		z /= radius;
	}
	
	handleX[idx] = x;
	handleY[idx] = y;
	handleZ[idx] = z;
		
	bang();
}

function add(x, y, z)
{
	if (num_points >= max_points)
		return;
	
	handleX.push(x);
	handleY.push(y);
	handleZ.push(z);
	
	num_points++;
	
	outlet(1, 1, num_points - 1);
	point_select = num_points - 1;
	bang();
}

function remove(idx)
{
	if (idx < 0 || idx >= num_points || !num_points)
		return;
		
	handleX.splice(idx, 1);
	handleY.splice(idx, 1);
	handleZ.splice(idx, 1);
	
	num_points--;
	
	if (idx >= point_select)
		point_select--;
	if (point_select < 0 && num_points)
		point_select = 0;

	outlet(1, 0, idx);
	bang();
}

function clear()
{
	handleX.length = 0;
	handleY.length = 0;
	handleZ.length = 0;
	
	num_points = 0;
	
	bang();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

function bang()
{
	mgraphics.redraw();
	
	if (point_select > -1)
		outlet(0, point_select, handleX[point_select], handleY[point_select], handleZ[point_select]);
}	



function clip(a, b, c)
{
	return a > c ? c : a < b ? b : a;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

function onclick(x, y, but, cmd, shift, capslock, option, ctrl)
{
    // cache mouse position for tracking delta movements
    
    var min_dist = 100000000;
    var compare_x, compare_y, compare_dist;

    handle_select = 0;
	var coords = sketch.screentoworld(x, y);
	x = coords[0];
	y = coords[1];

    last_x = x;    
    last_y = y;
        
	for (var i = 0; i < num_points; i++)
	{
		var draw_coords = calc_draw_coords(i);
 	
		compare_x = draw_coords[0] - 1.0;
    	compare_y = draw_coords[1];    
    	compare_x -= x;
    	compare_y -= y;
    	compare_dist = compare_x * compare_x + compare_y * compare_y;
        handle_dist = (1.0 + draw_coords[5] * z_scale) * handle_r * 1.1;
		handle_dist *= handle_dist;
		
    	if ((compare_dist < handle_dist) && (compare_dist < min_dist))
		{
			mode = 0;
			min_dist = compare_dist;
			handle_select = 1;
			point_select = i;
		}
		
		compare_x = draw_coords[3] + 1.0;
    	compare_y = draw_coords[2];  
    	compare_x -= x;
    	compare_y -= y;
    	compare_dist = compare_x * compare_x + compare_y * compare_y;
		handle_dist = (1.0 + (draw_coords[4] + 1.0) * y_scale) * handle_r * 1.1;
		handle_dist *= handle_dist;
		
    	if ((compare_dist < handle_dist) && (compare_dist < min_dist))
    	{
			mode = 1;
			min_dist = compare_dist;
			handle_select = 1;
			point_select = i;	
		}
	}
    
	if (handle_select)
	{
		var radius = Math.sqrt(handleX[point_select] * handleX[point_select] + handleY[point_select] * handleY[point_select] + handleZ[point_select] * handleZ[point_select]) + 0.00001;
    	
		if (radius > 0.0)
		{
			azimuth = Math.atan2(handleX[point_select], handleY[point_select]);
			elevation = Math.asin(clip(handleZ[point_select] / radius, 0, 1));
			
			var s = 1.0 / (Math.sqrt(handleX[point_select] * handleX[point_select] + handleY[point_select] * handleY[point_select]));
			
		if (isFinite(s))		
		{
			edit_vector[0] = handleX[point_select] * s;
			edit_vector[1] = handleY[point_select] * s; 
		}
		else
			edit_vector = [0, 1];
		}
		else
		{
			azimuth = 0;
			elevation = 0.0;
			edit_vector = [0, 1];
		}
				
		flip = handleX[point_select] < 0.0;
		
		last_shift = shift;
		mode = shift ? 1 - mode : mode;
        max.message("hidecursor");        
	}
	
	bang();
}
onclick.local = 1; 


function ondrag(x, y, but, cmd, shift, capslock, option, ctrl)
{
	if (handle_select)
    {
		var coords = sketch.screentoworld(x, y);
		var x = coords[0];
		var y = coords[1];
	
		// Relative changes
	
		x -= last_x;
		y -= last_y;
	
		last_x = coords[0];
    	last_y = coords[1];

		if (shift != last_shift)
		{
			mode = 1 - mode;
			last_shift = shift;
		}
			
		if (!mode)
		{
			handleX[point_select] += x / (area_r * (1 - space));
			handleY[point_select] += y / (area_r * (1 - space));

			var radius = Math.sqrt(handleX[point_select] * handleX[point_select] + handleY[point_select] * handleY[point_select] + handleZ[point_select] * handleZ[point_select]);
		    if (radius > 0.0)
			{
				azimuth = Math.atan2(handleX[point_select], handleY[point_select]);
				elevation = Math.asin(clip(handleZ[point_select] / radius, 0, 1));

				var s = 1.0 / (Math.sqrt(handleX[point_select] * handleX[point_select] + handleY[point_select] * handleY[point_select]));
				
				edit_vector[0] = handleX[point_select] * s;
				edit_vector[1] = handleY[point_select] * s; 
			}

			flip = handleX[point_select] < 0.0;
			
			if (radius > 1.0)
			{
				handleX[point_select] = Math.sin(elevationMax - elevation) * Math.sin(azimuth);
 				handleY[point_select] = Math.sin(elevationMax - elevation) * Math.cos(azimuth);
			}
		}
		else
		{
			var radius = Math.sqrt(handleX[point_select] * handleX[point_select] + handleY[point_select] * handleY[point_select] + handleZ[point_select] * handleZ[point_select]) + y;
			radius = clip(radius, 0.0, 1.0);

			elevation -= flip ? -x : x;
			elevation = clip(elevation, 0, elevationMax * 2);
			handleX[point_select] = radius * Math.sin(elevationMax - elevation) * Math.sin(azimuth);
 			handleY[point_select] = radius * Math.sin(elevationMax - elevation) * Math.cos(azimuth);
 			handleZ[point_select] = radius * Math.cos(elevationMax - elevation);
		}
		
		if (!but)
    	{
        	handle_select = 0;
        	max.message("showcursor");
		}
		
		bang();
    }
}
ondrag.local = 1; 



function ondblclick(x,y,but,cmd,shift,capslock,option,ctrl)
{
	var coords = sketch.screentoworld(x, y);
	x = coords[0];
	y = coords[1];
	
	var scale = area_r * (1 - space);
	
	var xl = (x + 1) / scale;
	var yb = y / scale;
	var radius = Math.sqrt(xl * xl + yb * yb);
	
	if (radius <= 1)
	{
		add(xl, yb, 0)
		return;
	}
	
	if (radius <= 1 / scale)
	{
		add(xl / radius, yb / radius, 0);
		return;
	}
	
	var xr = (x - 1) / scale;
	
	if (yb < 0)
	{
		if (yb > -space)
			yb = 0;
		else
			return;
	}
	
	radius = Math.sqrt(xr * xr + yb * yb);
	
	if (radius <= 1)
	{
		add(xr, 0, yb)
		return;
	}
	
	if (radius <= 1 / scale)
	{
		add(xr / radius, 0, yb / radius);
		return;
	}
}
ondblclick.local = 1;

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

function calc_draw_coords(idx)
{
	var select = handle_select && mode;
	
	// spacing
			
	var scale = area_r * (1 - space);
	var xdraw = handleX[idx] * scale; 
	var ydraw = handleY[idx] * scale;
	var zdraw = handleZ[idx] * scale;
	var xpdraw = !select ? xdraw : (handleX[idx] * edit_vector[0] + handleY[idx] * edit_vector[1]) * scale;
	var ypdraw = !select ? -handleY[idx] : handleY[idx] * edit_vector[0] + handleX[idx] * edit_vector[1];
	var zsdraw = handleZ[idx];
	
	if (select && flip)
		xpdraw *= -1.0;
		
	return [xdraw, ydraw, zdraw, xpdraw, ypdraw, zsdraw, point_select == idx, idx];
}


function sortZ(a, b)
{
	if (a[2] < b[2])
    	return -1;
 
  	if (a[2] > b[2])
    	return 1;

  return 0;
}


function sortY(a, b)
{
	if (a[4] < b[4])
    	return -1;
 
  	if (a[4] > b[4])
    	return 1;

  return 0;
}

function handle_colour(grad)
{
	handle_fill = [0, 0, 0, 0.8];
	
	for (var i = 0; i < 3; i++)
		handle_fill[i] = (handle_fill1[i] * (1 + grad)) % 1;
			//handle_fill[i] = handle_fill1[i] * (1 - grad) + handle_fill2[i] * grad;
	
	return handle_fill;
}

function paint()
{	
	with (mgraphics) {
		
		// Clear
		
		set_source_rgba(1, 1, 1, 0.75);
				
		rectangle(-2, 1, 4, 2);
		fill();
		
		var drawing_temp = new Array;
		var handle_s;
		
		// Calculate all coords for drwaing
		
		for (var i = 0; i < num_points; i++)
			drawing_temp[i] = calc_draw_coords(i);
			
		// set line width
		
		set_line_width(0.011);
		
		// draw areas

		set_source_rgba(area_fill);
		
		// N.B. y coords go bottom to top even in relative coords mode...
		
		ellipse(-(area_r + 1.0), area_r, area_r * 2, area_r * 2);
		fill();
		arc(1.0, 0.0, area_r, Math.PI, 0);		
		fill();
		rectangle(1.0 - area_r, 0, 2 * area_r, space);
		fill();

		// draw lines and origins
				
		if (handle_select && mode)
		{
			// draw lines
		
			set_source_rgba(origin_stroke);

			move_to(1, 0);
			line_to(drawing_temp[point_select][3] + 1.0, drawing_temp[point_select][2]);
			stroke();
		
			set_source_rgba(cross_stroke);
			
			var rad = area_r * (1 - space);
			move_to(-1.0 + edit_vector[0] * rad,  edit_vector[1] * rad);
			line_to(-1.0 - edit_vector[0] * rad, -edit_vector[1] * rad);
			stroke();
			move_to(1.0 - (area_r * (1 - space)), 0);
			line_to(1.0 + (area_r * (1 - space)), 0);
			stroke();
		}
		
		// draw origins
		
		set_source_rgba(origin_stroke);

		ellipse(-1.0 - origin_or, origin_or, origin_or * 2, origin_or *2);
		fill();
		ellipse( 1.0 - origin_or, origin_or, origin_or * 2, origin_or *2);
		fill();
		
		set_source_rgba(area_fill);

		ellipse(-1.0 - origin_ir, origin_ir, origin_ir * 2, origin_ir *2);
		fill();
		ellipse( 1.0 - origin_ir, origin_ir, origin_ir * 2, origin_ir *2);
		fill();
		
		// draw handles (lhs)
		
		drawing_temp.sort(sortZ);
		
		for (var i = 0; i < num_points; i++)
		{
			var draw_coords = drawing_temp[i];
				
			set_source_rgba((draw_coords[6]) ? selected_fill : handle_colour(draw_coords[7] / 3.1));				
			handle_s = (1.0 + draw_coords[5] * z_scale) * handle_r;
			ellipse(draw_coords[0] - (handle_s + 1.0), draw_coords[1] + handle_s, handle_s * 2, handle_s * 2);
			fill();
			
			set_source_rgba((draw_coords[6] && handle_select && !mode) ? active_fill : origin_stroke);
			ellipse(draw_coords[0] - (handle_s + 1.0), draw_coords[1] + handle_s, handle_s * 2, handle_s * 2);
			stroke();
		}
		
		// draw handles (rhs)
		
		drawing_temp.sort(sortY);
		
		for (var i = 0; i < num_points; i++)
		{
			var draw_coords = drawing_temp[i];
				
			set_source_rgba((draw_coords[6]) ? selected_fill : handle_colour(draw_coords[7] / 3.1));				
			handle_s = (1.0 + (draw_coords[4] + 1.0) * y_scale) * handle_r;
			ellipse(draw_coords[3] + (1.0 - handle_s), draw_coords[2] + handle_s, handle_s * 2, handle_s * 2);
			fill();
			
			set_source_rgba((draw_coords[6] && handle_select && mode) ? active_fill : origin_stroke);
			ellipse(draw_coords[3] + (1.0 - handle_s), draw_coords[2] + handle_s, handle_s * 2, handle_s * 2);
			stroke();
		}
	}
}


function forcesize(w,h)
{
	if (w != h * 2) {
		w = h * 2;
		box.size(w,h);
	}
}
forcesize.local = 1; 


function onresize(w,h)
{
	forcesize(w,h);
	mgraphics.redraw();
}
onresize.local = 1; 
