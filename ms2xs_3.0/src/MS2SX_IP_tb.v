//
// Verilog code for AXI4-Stream IP-Module of NTRU Serial Multiplier with M AUs 
// MS2XS_IP_tb.v - TestBench for ms2xs_v1_0
// santiago@imse-cnm.csic.es (31/10/2020)
//

`timescale 1ns / 1ps

module ms2xs_ip_tb ();

	parameter integer D_WIDTH = 32;
 	parameter integer S_COUNT = 32;
    parameter N = 11;
    parameter q = 64;
    parameter p = 3;
    parameter dr = 3;
    parameter M = 1;
	
    reg  din_axis_aclk;
    reg  din_axis_aresetn;
    reg  [D_WIDTH-1:0] din_axis_tdata;
    reg  din_axis_tvalid;
    reg  din_axis_tlast;
    reg  dout_axis_tready;
    wire din_axis_tready;
    wire dout_axis_tlast;
    wire dout_axis_tvalid;
    wire [D_WIDTH-1:0] dout_axis_tdata;
    wire mult_done;
 
	design_1_wrapper # (.N(N), .q(q), .p(p), .dr(dr), .M(M), .D_WIDTH(D_WIDTH), .S_COUNT(S_COUNT))
	  dut (
	    .din_axis_aclk(din_axis_aclk), .din_axis_aresetn(din_axis_aresetn),
		.din_axis_tdata(din_axis_tdata), .din_axis_tvalid(din_axis_tvalid), 
		.din_axis_tlast(din_axis_tlast), .din_axis_tready(din_axis_tready),
		.dout_axis_tready(dout_axis_tready), .dout_axis_tvalid(dout_axis_tvalid), 
		.dout_axis_tlast(dout_axis_tlast),   .dout_axis_tdata(dout_axis_tdata), 
		.mult_done(mult_done)
	  );
		
	initial
		begin
			din_axis_aresetn = 0;
			din_axis_tdata = 255;
			din_axis_tvalid = 0; 
			din_axis_tlast = 0;
			dout_axis_tready = 0;
			#500; 
			din_axis_aresetn = 1; 
			#380;
            din_axis_tvalid = 1;
            din_axis_tdata = 22*16+1*4+0; #100;    
            din_axis_tdata = 22*16+1*4+0; #100;
            din_axis_tdata = 53*16+0*4+1; #100;
            din_axis_tdata = 44*16+1*4+3; #100;
            din_axis_tdata = 29*16+0*4+0; #100;
            din_axis_tdata = 33*16+1*4+1; #100;
            din_axis_tdata = 38*16+3*4+3; #100;
            din_axis_tdata = 22*16+0*4+0; #100;
            din_axis_tdata = 43*16+0*4+1; #100;
            din_axis_tdata = 59*16+3*4+3; #100;
            din_axis_tdata = 17*16+0*4+0; #100;
            din_axis_tdata = 48*16+3*4+1; #100;		
 			din_axis_tdata =0;
            din_axis_tvalid = 0;
            #1500;
            dout_axis_tready = 1;
            #8500;
            din_axis_tvalid = 1;
            din_axis_tdata = 22*16+1*4+0; #100;    
            din_axis_tdata = 22*16+1*4+0; #100;
            din_axis_tdata = 53*16+0*4+1; #100;
            din_axis_tdata = 44*16+1*4+3; #100;
            din_axis_tdata = 29*16+0*4+0; #100;
            din_axis_tdata = 33*16+1*4+1; #100;
            din_axis_tdata = 38*16+3*4+3; #100;
            din_axis_tdata = 22*16+0*4+0; #100;
            din_axis_tdata = 43*16+0*4+1; #100;
            din_axis_tdata = 59*16+3*4+3; #100;
            din_axis_tdata = 17*16+0*4+0; #100;
            din_axis_tdata = 48*16+3*4+1; #100;        
            din_axis_tdata =0;
            din_axis_tvalid = 0;
            #1500;
            dout_axis_tready = 1;
		end
     
    // generate clock
    always     
        begin 
            din_axis_aclk=0; #50; din_axis_aclk=1; #50; 
		end
		 
             		  
    function integer clog2;
        input integer n;
            for (clog2=0; n>0; clog2=clog2+1)
                n = n >> 1;
    endfunction
    
endmodule
