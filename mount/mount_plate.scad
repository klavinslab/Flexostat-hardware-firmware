//12" minus laser cutoff is exactly a 300mm piece + 1 recess. 
sheet_height = 12*25.4;
//480mm beam - 15*2 for conneting beam width + 4.5*2 for insert+0.3 for laser 
sheet_width = 459.3; 

pump_x = 30;
pump_y = 50;

pv_y = sheet_height-35;
pv_spacing = (sheet_width-2*pump_x)/8;

$fn=40;
circ_comp=cos(180/30);

difference() {
  square([456,sheet_height], center=false);
  for (ii = [0:8]) {
    PV_holes([pump_x+ii*pv_spacing,pv_y]);
  }
  pump_holes([pump_x,pump_y]);
  translate([sheet_width/2,75]) mirror([1,0,0]) PCB_holes([0,0]);
}

module PCB_holes(LR_corner){
  hole_locs = [ [0.6,0.75],[0.6,2.2],[2.95,2.2], [3.6,0.75],
                [2.95,0.75],[3.6,2.2],[-0.05,0.75],[-0.05,2.2] ]*24.5;
  translate(LR_corner) rotate([0,0 ,90 ])
  for (loc = hole_locs) {
    translate(loc) circle(d=3.8*circ_comp);
  }
}

//pump_base = location of bottom pump mount hole
//should be about 2" above edge
module pump_holes(pump_base) {
  hole_locs = [[0,0],[0,20],[0,40],[-15/2,40+70],[-15/2,40+70+10],
    [ 15/2,40+70],[ 15/2,40+70+10]];

  translate(pump_base)
  for (loc = hole_locs) {
    translate(loc) circle(d=3.8*circ_comp);
  }
}

module PV_holes(center_loc) {
  translate(center_loc)
  union() {
    circle(d=20.3*circ_comp);
    translate([0,32/2])
      circle(d=3.8*circ_comp);
    translate([0,-32/2])
      circle(d=3.8*circ_comp);
  }
}
