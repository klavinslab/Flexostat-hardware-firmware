//options:
heater_cut_size = 0; //0=off -- Experimental
fan_stir = 1; //0=stepper motor stir, 1=20mm fan stir

//general parameters
tap_hole_632 = 2.95;
splitter_size = 12; //12mmx12mm microscope slide
laser_length = 17;
laser_dia = 7.1;
laser_path_dia = 5;
laser_path_height = 30;
wall_thickness = 1.5;

//tube parameters
tube_diameter = 25.7; //chemglass 25mmx100mm //actual dia 24.7
//tube_diameter = 12.2*2; //kimax 24mm cfuge tube //actual diameter 23.8mm
tube_holder_wall = wall_thickness; 
tube_holder_height = 100;
tube_window_width = 12;
tab_depth = 1;

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

//Stir motor parameters
hole_dist = 42;
motor_mount_angle = 30;
//
fan_hole_dist = 32;
fan_hole_dia = 0.1495*25.4+.1; //clearance fit for 6-32
magnet_depth = fan_stir==1? 5.1+tube_holder_wall : 15; //fan depth : stepper depth

//derived parameters
splitter_center_x = abs(tx_llc[0])+sensor_holesize[1]/2;
base_depth = splitter_size-sensor_depth+2;
outer_diameter = min(PCB_tube_hole_diameter-1,tube_diameter+2*tube_holder_wall);
if(outer_diameter>PCB_tube_hole_diameter-1.01) {
  echo("WARNING: tube diameter parameter may be too large.");
}

/////////////////////////////////
//Assemble the whole part.     //
/////////////////////////////////
difference() {
  union() {
    tube_holder(tube_diameter);
    translate([0,0,laser_path_height+tube_holder_wall]) 
      PCB_and_optics();
    rotate([0,0,motor_mount_angle]) translate([0,0,-magnet_depth]) 
      stir_holder(magnet_depth,tube_diameter,hole_dist,window=false,fan=fan_stir); // do window later
  }
  //negative parts of tube holder and laser/pcb holder
  tube_holder_negative_parts(tube_diameter);
  translate([0,0,laser_path_height+tube_holder_wall]) 
    laser(tap_hole_632);

  //windows TODO: refactor into tube_holder_negative_parts
  translate([-tube_window_width/2,-50,tube_holder_wall+10]) 
    cube([tube_window_width,100,tube_holder_height*0.70], center=false);
  if(fan_stir==0){
    translate([-tube_window_width/2,-50,-10]) 
      cube([tube_window_width,100,10], center=false);
  }
  if (heater_cut_size>0) {
    translate([0,0,heater_cut_size/2])
    rect_helix(10,tube_holder_height,tube_diameter/2+heater_cut_size/2,heater_cut_size,300);
  }
}
tube_tabs(tube_diameter);

/////////////////////////////////
//Modules                      //
/////////////////////////////////

module stir_holder(h,td,hole_offset,window=false,fan=0){
  $fn=100;
  if (fan==0) {
    difference() {
      union() {
        difference() {
          hull() {
            for(i = [hole_offset/2, -hole_offset/2]) {
              translate([i,0,0])
                cylinder(h=h,d=tap_hole_632+wall_thickness*2, center=false);
            } 
          }
          for(i = [hole_offset/2, -hole_offset/2]) {
            translate([i,0,0])
              cylinder(h=h,d=tap_hole_632, center=false);
          } 
        }
        cylinder(h=h, d=outer_diameter, center = false);
      }
      cylinder(h=h, d=td);
      if(window) {
        translate([-tube_window_width/2,-50,5]) 
          cube([tube_window_width,100,h-5], center=false);
      }
    }
  }
  else { //fan holder
    translate([0,0,tube_holder_wall])
    difference() {
      translate([-20,-20]) v_rounded_cube([40,40,h],4);
      for(x = [fan_hole_dist/2,-fan_hole_dist/2]) {
        for(y = [fan_hole_dist/2,-fan_hole_dist/2]) {
          translate([x,y,0])
            cylinder(h=2.1*h,d=fan_hole_dia , center=true,$fn=30);
        }
      }
      cylinder(h=(h-tube_holder_wall)*2 ,d=32,center=true);
    }
  }
}

module tube_holder(td) {
  $fn=200;
  //tube part
  cylinder(h=tube_holder_height, d=outer_diameter, center = false);
}

module tube_tabs(td) {
  $fn=100;
  //the tabs
  translate([0,0,tube_holder_height-5])
    intersection() {
      cube([100,10,10],center = true);
      difference() {
        cylinder(h=10,d=td+tube_holder_wall);
        //make cone 0.1 wider and taller to work around preview glitch
        translate([0,0,-0.1])cylinder(h=10+0.1,d1=td+0.1,d2=td-tab_depth);
      }
    }
}

module tube_holder_negative_parts(td) {
  $fn=200;
  //actual tube hole
  translate([0, 0, tube_holder_wall])
    cylinder(h=tube_holder_height, d=td);
  //tab cutouts
  //translate([-50,-1.5/2+5,tube_holder_height-10]) cube([100,1.5,10]);
  //translate([-50,-1.5/2-5,tube_holder_height-10]) cube([100,1.5,10]);
}

module PCB_and_optics() {
  difference() {
    PCB_body(rx_llc[0]-2,laser_end_x-1);
    PCB_holes(tap_hole_632);
    beam_splitter(splitter_size,0.6);
    laser(tap_hole_632);
    bevel(30+90,10,28,45);
    bevel(-30-90,10,28,45);
  }
}

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

module beam_splitter(size,thick) {
  color("blue")
  translate([0,0,-((size)/2)+sensor_depth-1/2])
    translate([splitter_center_x,0,0]) rotate([0,0,-45])
      union() {
        $fn = 10;
        cube([thick,size+1,size+1],center = true);
        translate([0,-(size)/2]) cylinder(h=size+1,d=2*thick,center = true);
        translate([0,(size)/2]) cylinder(h=size+1,d=2*thick,center = true);
      }

}

module laser(screw_hole_dia) {
  color("red") translate([rx_llc[0],0,0]) rotate([0,90,0]) 
  cylinder(h=abs(rx_llc[0])+splitter_center_x + splitter_size/sqrt(2)/2+1,
    d=laser_path_dia , center=false,$fn=30);
  color("red") translate([abs(tx_llc[0])+sensor_holesize[1]/2,0,0])
    rotate([90,0,0])
      cylinder(h=abs(tx_llc[1]),d=laser_path_dia , center=false,$fn=30);
  color("pink") translate([splitter_center_x + splitter_size/sqrt(2)/2+1,0,0])
    rotate([0,90,0])
      cylinder(h=laser_length,d=laser_dia+0.2,center=false,$fn=300);
  for (i = [0:360/3:359]) {
    rotate([i,0,0]) color("gray") translate([PCB_rect_holecenter[0],0,0]) 
      cylinder(h=20,d=screw_hole_dia,center=false,$fn=30);
  }
}
module bevel(angle,r,x1,x2) {
  rotate([angle,0,0]) translate([x1,-10,r])
    cube([abs(x2-x1),20,20],center = false);
}

module PCB_body(x1,x2) { //length of x in each dir 
  translate([x1,-PCB_height/2,-base_depth])
    cube([abs(x2-x1),PCB_height,sensor_depth+base_depth]);
  color("green") translate([PCB_rect_holecenter[0]-PCB_rect_holesize[0]/2
    ,PCB_rect_holecenter[1]-PCB_rect_holesize[1]/2,sensor_depth])
      cube([PCB_rect_holesize[0],PCB_rect_holesize[1],3],center = false);
}

module rect_helix(turns,height,r,cutsize,slices=100) {
  theta = atan((height/turns)/(PI*2*r));
  intersection() {
    for (i = [1:slices]) {
      hull() {
        rotate([0,0,turns/slices*360*i])
        translate([0,r,height/slices*i])
        rotate([0,theta,0])
        cube(cutsize,center=true);

        rotate([0,0,turns/slices*360*(i-1)])
        translate([0,r,height/slices*(i-1)])
        rotate([0,theta,0])
        cube(cutsize,center=true);
      }
    }
    cylinder(h=height,r=r*2);
  }
}


module v_rounded_cube(xyz,r,$fn=30) {
  z = xyz[2];
  y = xyz[1];
  x = xyz[0];
  translate([r,r])
  hull() {
    cylinder(h=z,r=r);
    translate([0,y-2*r])
    cylinder(h=z,r=r);
    translate([x-2*r,0])
    cylinder(h=z,r=r);
    translate([x-2*r,y-2*r])
    cylinder(h=z,r=r);
  }
}