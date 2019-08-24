inlets = 1;
outlets = 1;
setinletassist(0, "Some message");
function bang()
{
   var p = this.patcher;
   while (p.parentpatcher)  // If I have a parent, keep going up
      {
         p = p.parentpatcher;
      }
	var temp = p.name;
	temp = temp.slice(0, -8);
   outlet(0, temp);
}