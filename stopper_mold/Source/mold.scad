//1 = top
//2 = middle
//3 = bottom
//anything else = all
// You must render and print all 3
part = 3;
tube_id = 22.2; // chemglass 25mm tube -> 22.2, kimble cfuge -> 20.3

stopper_height = 20;
stopper_d1 = tube_id-0.3;
stopper_d2 = tube_id+5;
sstube_d = 2.2+0.4; //small holes need extradilation
shell_thickness = 1.6;
sstube_loc = [[0,0],[7.5,0],[-7.5,0]];
sstube_len = [40,10,10]; //tube length below stopper
screw_hole_d = 0.156*25.4; 

epsilon = 0.01;

//derived
shell_t = shell_thickness; //shortcut
outer_diameter = stopper_d2+2*shell_t;
intersection() {
	difference() {
		union() {
			if (part != 3) {//shouldn't need this but i do.
				translate([0, 0,-shell_t]) 
					cylinder(h=stopper_height+shell_t, d=outer_diameter, $fn=50);
			}
			sstube_holes(sstube_loc,sstube_len,sstube_d+2*shell_t);
			translate([0,0,-2*shell_t]) clampy_part();
		}
		//stopper
		cylinder(h=stopper_height, d1=stopper_d1,d2=stopper_d2, $fn=100);
		//ss tube holes
		sstube_holes(sstube_loc,sstube_len,sstube_d);
		//make most of the tube holes a bit bigger.
		translate([0,0,-shell_t]) sstube_holes(sstube_loc,sstube_len,sstube_d+0.4);
	}
	if (part == 1) {
		cylinder(h=300,r=300);
	}
	if (part == 2) {
		translate([0, 0, -shell_t])
		cylinder(h=shell_t,r=300);
	}
	if (part == 3) {
		translate([0, 0, -shell_t-epsilon]) mirror([0,0,1])
		cylinder(h=300,r=300);
	}
}

module sstube_holes(loc,t_len,thick) {
	mirror([0,0,1])
	for (ii = [0:len(loc)]){ 
		translate(loc[ii]) 
		  cylinder(h=t_len[ii] , d=thick, $fn=50);
	}
}

module clampy_part() {
	$fn=30;
	difference() {
		hull() {
			translate([2*shell_t+(outer_diameter)/2,0])
			cylinder(h=3*shell_t,d=screw_hole_d+2*shell_t);
			translate([-(2*shell_t+(outer_diameter)/2),0])
			cylinder(h=3*shell_t,d=screw_hole_d+2*shell_t);
		}
		translate([2*shell_t+(outer_diameter)/2,0])
		cylinder(h=3*shell_t,d=2*shell_t);
		translate([-(2*shell_t+(outer_diameter)/2),0])
		cylinder(h=3*shell_t,d=2*shell_t);
	}
}