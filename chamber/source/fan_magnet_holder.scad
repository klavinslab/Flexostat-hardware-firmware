thickness =1.6;
diameter = 25.2+0.4;
magnet_hole_dia = 6.6;
magnet_hole_depth = 4.2;
top_height = 4.2;
overall_height = 10;
hole_offset = 16/2;

translate([0, 0, overall_height]) rotate([180, 0, 0])
difference() {
  cylinder(h=overall_height,d=diameter+2*thickness);
  cylinder(h=(overall_height-top_height)*2, d=diameter, center=true,$fn=60);
  for (i = [hole_offset,-hole_offset]) {
    translate([i,0,overall_height-magnet_hole_depth]) 
      cylinder(h=magnet_hole_depth*2,d=magnet_hole_dia,center=true,$fn=100);
    translate([i,0,overall_height-0.3]) 
      cylinder(h=magnet_hole_depth,d2=magnet_hole_dia+0.6,d1=magnet_hole_dia,center=true,$fn=100);
  }
  for (i=[0:4]) {
  	rotate([0,0,360/5*i])
  	cube([10,100,(overall_height-top_height)]);
  }
}

//"tool" for pressing magnets
//translate([30,0])
	//cylinder(h=7,d=diameter-1);


