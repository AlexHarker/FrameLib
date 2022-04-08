inlets=1;
outlets=1;

this.box.message("border", 0);
this.box.message("ignoreclick", 1);

mgraphics.init();
mgraphics.relative_coords = 0;
mgraphics.autofill = 0;


function paint()
{
	with (mgraphics) 
   	{
		fillcolor = [ 1, 1, 1, 1];
		strokecolor = [ 0.75, 0.75, 0.75, 1];
		thickness = 2;
		adjust = thickness / 2;
		
		width = (box.rect[2] - box.rect[0]);
		height = (box.rect[3] - box.rect[1])
		
		flip = jsarguments[1];
		
		edge1 = flip ? 0 + adjust : width - adjust;
		edge2 = flip ? width - adjust : 0 + adjust;
		top = 0 + adjust;
		bottom = height;
   		
		// Baground
		
		move_to(edge1, bottom);
   		line_to(edge1, top - adjust);
   		line_to(edge2, bottom);

		set_source_rgba(fillcolor);
		fill();
		
		// Strokes
		
		move_to(edge1, bottom);
   		line_to(edge1, top - adjust);
   		line_to(edge2, bottom);

		set_source_rgba(strokecolor);
		stroke();
		
		// Cleanup
		
		set_source_rgba(fillcolor);
		move_to(0, bottom - adjust);
   		line_to(width, bottom - adjust);
		stroke();
		rectangle(0, height - (thickness - adjust), width, thickness);
		fill(); 
	}
}