`include "bleDefines.v"
`include "radioDefines.v"
`include "spi_h.v"

`define	VCC	1'b1

module topModule (
input		clk_in, //125 mhz
input		top_rst_n,

input       rxclk,//lvds
input       rxd09,//lvds

output serial_iq,
output serial_clk
//output clock_out,

//output reg		LED,
//output  reg LED1,
//output  reg LED2,
//output  reg LED3
////output pll_clock 
);

//--------------------------------------------------------------
// Nets
//----------------------------------------------------------------
parameter [0:0] VSS = 1'b0;
parameter [0:0] VCC = 1'b1;

wire			clkDivider_lock;


reg				sin_clkEN;
reg				sin_rst;
reg				sin_theta;
wire			sin_sine;
wire			sin_cosine;

wire				fskModule_symVal;
wire [`SinSize-1:0]	fskModule_I;
wire [`SinSize-1:0]	fskModule_Q;
wire				fskModule_symDone;
wire				fskModule_start;

wire [`ILength-1:0] IQSerializer_I;
wire [`QLength-1:0]	IQSerializer_Q;
wire 				IQSerializer_start;



reg								pktReader_ready;
wire	[`BLE_Mem_Addr-1: 0]	pktReader_mem_addr;
wire							pktReader_done;


//wire top_rst_n;
//-------------------------------------------------------------------
// Constant assignments
//--------------------------------------------------------------------
//assign	top_rst_n = clockDivider_clkLock;
assign	osc_en = 1'b1;
//assign 	pll_clki = top_clk;
//assign top_rst_n =1'b1;  ////////////////////////////////////////////////////////// I added this to remove complete reset pin connection

//assign IQSerializer_I = {fskModule_I, 1'b0};
//assign IQSerializer_Q = {fskModule_Q, 1'b0};

assign IQSerializer_I = 14'b00000000000000;
assign IQSerializer_Q = 14'b11111111111111;


//assign serial_clk = clk_out1;

//debugg
//assign top_test1	= clkDivider_lock;
//assign top_test2	= clkDivider_clko;
//assign top_test3	= fskModule_symVal;
//assign top_test6	= fskModule_symDone;

//------------------------------------------------------------------
// State
//------------------------------------------------------------------
//LED

//always @(*) begin
//	if (top_rst_n  == 1'b1) begin
//		LED= 1'b0;
//	end else begin
//		LED = 1'b1;
//	end
//end

//always @(*) begin
//	if (IQSerializer_start  == 1'b1) begin
//		LED3= 1'b0;
//	end else begin
//		LED3 = 1'b1;
//	end
//end


//reg [25:0] hb1;
//always @(posedge serial_clk) begin
//        hb1 <= hb1 + 1'b1;     
//        LED3 <= hb1[25];     
//end
//reg [25:0] hb2;
//always @(posedge serial_iq) begin   
//        hb2 <= hb2 + 1'b1;     
//        LED2 <= hb2[25];     
//end

//--------------------------------------------------------------------
 clk_wiz_0 clk_64M
   (
    // Clock out ports
    .clk_out1(clk_out1),     // output clk_out1
    // Status and control signals
    .reset(1'b0), // input reset
    .locked(locked),       // output locked
   // Clock in ports
    .clk_in1(clk_in)      // input clk_in1
);

//wire clk_in_ibuf;
//wire clk_out1;

//IBUF ibuf_clk (.I(rxclk), .O(clk_in_ibuf));  
//BUFG bufg_clk (.I(clk_in_ibuf), .O(clk_out1));     

//------------------------------------------------------------------
// Component instances
//-------------------------------------------------------------------
clockDivider clockDivider_0(
	.clk      (clk_out1),
	.pll_lock (locked),
	.clkOut   (clkDivider_clko),
	.clkLock  (clkDivider_lock) 
);

 



/* ######################################################
Use these two for fix pre-loaded packet.
###################################################### */
packetCounter counter_0(
	.clk       (clkDivider_clko),
	.clkLock   (clkDivider_lock),
	.countDone (counter_0_countDone) 
);

packetGenerator packetGen_0(
	.rst_n(counter_0_countDone),
	.clk(clockDivider_clko),
	.symDone(fskModule_symDone),
	.start(fskModule_start),
	.symVal(fskModule_symVal)
);


FSKModulator fskModule_0(
	.clk(clkDivider_clko),
	.rst_n(clkDivider_lock),
	.enable(fskModule_start),
	.symVal(fskModule_symVal),
	.FSK_I(fskModule_I),
	.FSK_Q(fskModule_Q),
	.symDone(fskModule_symDone),
	.start(IQSerializer_start)
);


IQSerializer IQSerializer_0(
	.clk(clk_out1),
	.start(IQSerializer_start),
	.I(IQSerializer_I),
	.Q(IQSerializer_Q),
	.serial_N(serial_iq),
	.serial(),
	.serial_clk(serial_clk)
);


endmodule