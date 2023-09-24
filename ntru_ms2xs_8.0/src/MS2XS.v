//
// Verilog code for AXI4-Stream IP-Module of NTRU Serial Multiplier with M AUs 
// MS2XS.v - Top_level
// camacho@imse-cnm.csic.es (24/09/2023)
//

`timescale 1ns / 1ps

module ms2xs #
  (
	parameter integer D_WIDTH = 32,
 	parameter integer S_COUNT = 32,
    parameter N = 11,
    parameter q = 2048,
    parameter p = 3,
    parameter M = 1,
    parameter max_cycles = 7
  )(
    input  clk,
    input  reset,
    input  [D_WIDTH-1:0] din_tdata,
    input  din_tvalid,
    input  din_tlast,
    input  dout_tready,
    output din_tready,
    output dout_tlast,
    output dout_tvalid,
    output [D_WIDTH-1:0] dout_tdata,
    output mult_done
  );

  // Input data signals
    wire [clog2(q-1)-1:0] data_h;
    wire [clog2(p-1)-1:0] data_r;
    wire [clog2(q-1)-1:0] data_m;
    assign data_h = din_tdata[clog2(q-1)+3:4];       
    assign data_r = din_tdata[3:2];
 	assign data_m = (din_tdata[1]) ? q-1 : din_tdata[1:0];
       
  // Operation signals    
    wire load;
    wire start;
    wire read;

  // axis_in signals
    wire [clog2(N-1)-1:0] write_addr_h;    
    wire [clog2($ceil(1.0*N/M)-1)+clog2(M-1)-1:0] write_addr_m;    
    wire write_en;
    wire write_done;
         
  // axis_out signals
    wire [clog2($ceil(1.0*N/M)-1)+clog2(M-1)-1:0] read_addr;    
    wire read_en;
    wire read_done;
    
  // Control signals
    wire [clog2(N-1)-1:0] addr_r;
    wire [clog2(N-1)-1:0] addr_h;
    wire [clog2(N-1)-1:0] addr_e;
    wire operate;
    wire end_op;
    
  // Mems signals
    wire [clog2(p-1)-1:0]  r;
    wire [M*clog2(q-1)-1:0] h;
    wire [M*clog2(q-1)-1:0] e;
	wire [clog2(q-1)-1:0]   eo;

  // AUs signals
    wire [M*clog2(q-1)-1:0] e_out;
    
    // Num_r dist z
   reg  [clog2(N)-1:0] nnz;
   
   //selection
   wire [1:0] selection;
   assign selection = din_tdata[25:24];   
	
	axis_in #(.N(N), .M(M), .D_WIDTH(D_WIDTH))
	  din (
	    .clk(clk), .reset(reset),
	    .din_tvalid(din_tvalid), .din_tlast(din_tlast), .din_tready(din_tready),
		.write_addr_h(write_addr_h), .write_addr_m(write_addr_m),
	    .write_en(write_en), .write_done(write_done)
	  );
	    
	axis_out #(.N(N), .M(M), .D_WIDTH(D_WIDTH), .S_COUNT(S_COUNT))
      dout (
        .clk(clk), .reset(reset), .read(read),
        .dout_tready(dout_tready), .dout_tvalid(dout_tvalid), .dout_tlast(dout_tlast),
        .read_en(read_en), .read_addr(read_addr), .read_done(read_done)
      );
    generate  
        if(max_cycles == N)
            control_womaxcycles #(.N(N), .q(q), .p(p), .M(M)) 
            Control_M (
            .clk(clk), .rst(reset), .start_op(start), .r(r),
            .addr_r(addr_r), .addr_h(addr_h), .addr_e(addr_e),
            .operate(operate), .end_op(end_op)
            );  
        else   
            control #(.N(N), .q(q), .p(p), .M(M), .max_cycles(max_cycles)) 
            Control_M (
            .clk(clk), .rst(reset), .start_op(start), .r(r), .nnz(nnz),
            .addr_r(addr_r), .addr_h(addr_h), .addr_e(addr_e),
            .operate(operate), .end_op(end_op)
            );
    endgenerate

    mems #(.N(N), .q(q), .p(p), .M(M)) 
      Mems (
        .clk(clk), .load(load), .operate(operate), .read(read),
        .addr_r_w(write_addr_h), .data_r(data_r), .addr_r_r(addr_r), 
        .addr_h_w(write_addr_h), .data_h(data_h), .addr_h_r(addr_h), 
        .addr_m_l(write_addr_m), .data_m(data_m),
        .addr_e(addr_e), .data_e(e_out), .addr_dout(read_addr),
        .r(r), .h(h), .e(e), .eo(eo)
      );

    aus #(.N(N), .q(q), .p(p), .M(M)) 
      AUs (
        .h(h), .r(r), .e(e), .e_out(e_out)
    );
    
    assign load  = write_en;
    assign start = write_done;
    assign read  = end_op;
    
    assign dout_tdata [D_WIDTH-1:clog2(q-1)+2] = {(D_WIDTH-clog2(q-1)-2){1'b0}};
    assign dout_tdata[clog2(q-1)+1:clog2(q-1)] = selection;
    assign dout_tdata[clog2(q-1)-1:0] = (read) ? eo : {clog2(q-1){1'b0}};
    
    assign mult_done = read_done;
 
     // 
     // Counter nnz -> Conocer numeros distintos de 0 en r
     //
    always @(posedge clk) 
    begin
        if (!reset) nnz <= {clog2(N){1'b0}};
        else begin
            if (load) begin
                if (data_r != 0) nnz <= nnz + 1'b1;
                else nnz <= nnz;
            end
        end
    end
    
  // clog2 function
    function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction

endmodule
