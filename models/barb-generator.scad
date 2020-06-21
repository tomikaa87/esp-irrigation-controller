$fn = 80;

function width_of_output_ports(count, diameter, spacing) =
    count * diameter + (count - 1) * spacing;

function x_offset_of_output_port(index, count, diameter, spacing, container_w) =
    let(outp_total_w = width_of_output_ports(count, diameter, spacing))
    index * (diameter + spacing) + (container_w - outp_total_w) / 2 + diameter / 2;

module vault(
    width,
    height,
    depth,
    wall_th,
    input_port_id,
    output_port_id,
    output_port_count,
    output_port_spacing
) {
    difference() {
        cube([width, depth, height]);
        
        inside_w = width - 2 * wall_th;
        inside_h = height - 2 * wall_th;
        inside_d = depth - 2 * wall_th;
        hull_r = inside_d / 2;

        translate([wall_th, wall_th, wall_th]) {
            hull() {
                intersection() {
                    translate([0, hull_r, inside_h - hull_r]) rotate([0, 90, 0]) {
                            cylinder(r = hull_r, h = width - wall_th * 2);
                    }
                    
                    // Cut the other half of the cylinder
                    translate([0, 0, inside_h - hull_r]) {
                        cube([inside_w, inside_d, hull_r]);
                    }
                }
             
                cube([inside_w, inside_d, inside_h * 0.1]);
            }
        }
        
        // Make a hole for the input port
        translate([width / 2, depth / 2, -0.01]) {
            cylinder(d = input_port_id, h = wall_th + 0.02);
        }
        
        // Make hole(s) for the output port(s)
        for (i = [0 : output_port_count - 1]) {
            x_offset = x_offset_of_output_port(i, output_port_count, output_port_id, output_port_spacing, width);
            translate([x_offset, depth / 2, height / 2]) {
                cylinder(d = output_port_id, h = 20);
            }
        }
    }
}

module port(
    height,
    outer_d,
    inner_d,
    barb_th = 2,
    barb_offset = 5,
    barb_count = 3,
    slope_ratio = 0.15
) {
    difference() {
        union() {
            cylinder(h = height, d = outer_d);
            
            barb_section_h = height - barb_offset;
            barb_h = barb_section_h / barb_count;

            // Make barbs
            for (i = [0 : barb_count - 1]) {
                translate([0, 0, barb_h * i + barb_offset]) {
                    // Make a slope to avoid using supports
                    cylinder(h = barb_h * slope_ratio, d1 = outer_d, d2 = outer_d + barb_th);
                    // Make the barb
                    translate([0, 0, barb_h * slope_ratio]) {
                        cylinder(h = barb_h * (1 - slope_ratio), d1 = outer_d + barb_th, d2 = outer_d);
                    }
                }
            }
        }
        
        translate([0, 0, -0.01]) {
            cylinder(h = height + 0.02, d = inner_d, $fn = 60);
        }
    }
}

module adapter(
    input_port_od = 12,
    input_port_id = 8.5,
    input_port_h = 30,
    output_port_count = 6,
    output_port_od = 12,
    output_port_id = 8.5,
    output_port_h = 30,
    output_port_spacing = 12,
    vault_wall_th = 2
) {
    vault_w = width_of_output_ports(output_port_count, output_port_od, output_port_spacing);
    vault_h = 13;
    vault_d = output_port_od + vault_wall_th * 4;

    union() {
        // The vault
        translate([0, -vault_d / 2, input_port_h - vault_wall_th + 0.1]) {
            vault(
                width = vault_w,
                height = vault_h,
                depth = vault_d,
                wall_th = vault_wall_th,
                input_port_id = input_port_id,
                output_port_id = output_port_id,
                output_port_count = output_port_count,
                output_port_spacing = output_port_spacing
            );
        }

        // Input port
        rotate([180, 0, 0]) translate([vault_w / 2, 0, -input_port_h]) {
            port(input_port_h, 12, 8.5, barb_offset = 6, barb_count = 3);
        }
        
        // Output port(s)
        for (i = [0 : output_port_count - 1]) {
            x_offset = x_offset_of_output_port(i, output_port_count, output_port_id, output_port_spacing, vault_w);
            translate([x_offset, 0, input_port_h + vault_h - 2 * vault_wall_th]) {
                port(
                    output_port_h,
                    output_port_od,
                    output_port_id,
                    barb_offset = 6,
                    barb_count = 3
                );
            }
        }
    }
}

adapter();