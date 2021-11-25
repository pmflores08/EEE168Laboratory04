#Project Name and Directory
set projectName Laboratory_04
set projectDir /nfs/home/peter.john/$projectName
set designName design_1

#Project Creation
create_project $projectName $projectDir -part xc7a35ticsg324-1L

#Board Specification
set_property board_part digilentinc.com:arty-a7-35:part0:1.1 [current_project]
create_bd_design "$designName"

#Clock Generator
create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:5.3 clk_wiz_0

#Clocking Wizard Connections
apply_board_connection -board_interface "sys_clock" -ip_intf "clk_wiz_0/clock_CLK_IN1" -diagram "$designName"
set_property -dict [list CONFIG.RESET_TYPE {ACTIVE_LOW} CONFIG.RESET_PORT {resetn}] [get_bd_cells clk_wiz_0]
apply_bd_automation -rule xilinx.com:bd_rule:board -config {Board_Interface "reset ( System Reset ) " } [get_bd_pins clk_wiz_0/resetn]

#Microblaze Processor
create_bd_cell -type ip -vlnv xilinx.com:ip:microblaze:9.6 microblaze_0
apply_bd_automation -rule xilinx.com:bd_rule:microblaze -config {local_mem "64KB" ecc "None" cache "None" debug_module "Debug Only" axi_periph "Enabled" axi_intc "0" clk "/clk_wiz_0/clk_out1 (100 MHz)" } [get_bd_cells microblaze_0]

#Board Progress 01
regenerate_bd_layout

#USB UART Module
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite:2.0 axi_uartlite_0
apply_board_connection -board_interface "usb_uart" -ip_intf "axi_uartlite_0/UART" -diagram "$designName"

#LED
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0
apply_board_connection -board_interface "led_4bits" -ip_intf "axi_gpio_0/GPIO" -diagram "$designName"

#Timer Module
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_timer:2.0 axi_timer_0

#Peripheral Connections to Microprocessor
apply_bd_automation -rule xilinx.com:bd_rule:board -config {Board_Interface "reset ( System Reset ) " } [get_bd_pins rst_clk_wiz_0_100M/ext_reset_in]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/microblaze_0 (Periph)" Clk "Auto" } [get_bd_intf_pins axi_uartlite_0/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/microblaze_0 (Periph)" Clk "Auto" } [get_bd_intf_pins axi_gpio_0/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/microblaze_0 (Periph)" Clk "Auto" } [get_bd_intf_pins axi_timer_0/S_AXI]

#Board Progress 02
regenerate_bd_layout

#Interrupt Controller
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_intc:4.1 axi_intc_0
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config {Master "/microblaze_0 (Periph)" Clk "Auto" } [get_bd_intf_pins axi_intc_0/s_axi]

#Interrupt Connections
create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0
connect_bd_net [get_bd_pins axi_timer_0/interrupt] [get_bd_pins xlconcat_0/In0]
connect_bd_net [get_bd_pins axi_uartlite_0/interrupt] [get_bd_pins xlconcat_0/In1]
connect_bd_net [get_bd_pins xlconcat_0/dout] [get_bd_pins axi_intc_0/intr]
connect_bd_intf_net [get_bd_intf_pins axi_intc_0/interrupt] [get_bd_intf_pins microblaze_0/INTERRUPT]

#Initial Validation and Saving of Base Microprocessor Design
regenerate_bd_layout
validate_bd_design
save_bd_design
