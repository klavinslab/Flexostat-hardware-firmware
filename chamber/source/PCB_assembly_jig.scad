
//general parameters
tap_hole_632 = 2.95;
splitter_size = 12; //12mmx12mm microscope slide
laser_length = 17;
laser_dia = 7.1;
laser_path_dia = 5;
laser_path_height = 30;
wall_thickness = 1.5;



//PCB parameters.  These can't change without changing the PCB
sensor_holesize = [3,5];
sensor_depth = 3.8;
rx_llc = [-21.32,-2.5]; //lower left corner of RX sensor hole
tx_llc = [0.9*25.4-sensor_holesize[1]/2,-9.89]; //lower left corner of TX sensor hole
PCB_hole_locations = [[-20.32,7.62],[-20.32,-7.62],[1.2*25.4,-8.89]];
PCB_rect_holesize = [7,7];
PCB_rect_holecenter = [1.3*25.4,0];
PCB_height=25;
ethernet_ulc = [(2-0.55)*25.4,-3-1];
ethernet_size =[17,17];
ethernet_depth = 2;
PCB_tube_hole_diameter = 30; 
laser_end_x = 43;   

difference() {
    translate([5,0,5.6/2]) cube([60,PCB_height-3,5.6],center = true);
    translate([0,0,2]) PCB_holes(tap_hole_632);
}
translate([5,0,-1]) cube([60,PCB_height-3,2],center = true);
 

module PCB_holes(screw_hole_dia) {
  $fn=200;
  linear_extrude(height=10,center = true ,convexity=10  )
  union() {
    translate(rx_llc)
      square(sensor_holesize,center=false);
    translate(tx_llc)
      square([sensor_holesize[1],sensor_holesize[0]],center=false);
  }
  linear_extrude(height=40,center = true ,convexity=10  )
    for (xy = PCB_hole_locations) {
      translate(xy)
        circle(d=screw_hole_dia);
    }
  color("gray") translate([ethernet_ulc[0],
    ethernet_ulc[1]-ethernet_size[1],
    sensor_depth-ethernet_depth])
      cube([ethernet_size[0],ethernet_size[1],ethernet_depth+1],center = false);
}