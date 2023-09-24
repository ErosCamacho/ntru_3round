//
// Verilog code for AXI4-Stream IP-Module of NTRU Serial Multiplier with M AUs 
// Mems.v - Parameters and results memories
// camacho@imse-cnm.csic.es (24/09/2023)
//

module mems # 
  (
    parameter N = 541,
    parameter q = 2048,
    parameter p = 3,
    parameter M = 2
  )( 
    input clk,
    input load,
    input operate,
    input read,
    input [clog2(N-1)-1:0] addr_h_r,
    input [clog2(N-1)-1:0] addr_h_w,
 	input [clog2(N-1)-1:0] addr_r_r,
    input [clog2(N-1)-1:0] addr_r_w,
    input [clog2($ceil(1.0*N/M)-1)+clog2(M-1)-1:0] addr_m_l,
    input [clog2($ceil(1.0*N/M)-1)+clog2(M-1)-1:0] addr_dout,
    input [clog2($ceil(1.0*N/M)-1)-1:0] addr_e,
    input [clog2(q-1)-1:0] data_h,
    input [clog2(p-1)-1:0] data_r,
    input [clog2(q-1)-1:0] data_m,
    input [M*clog2(q-1)-1:0] data_e,    
	output wire [clog2(p-1)-1:0] r,
	output wire [M*clog2(q-1)-1:0] h,
	output wire [clog2(q-1)-1:0] eo,
	output wire [M*clog2(q-1)-1:0] e
  );

  // Mems_h
    mems_h #(.N(N), .q(q), .p(p), .M(M)) 
      mems_h(
        .clk(clk), .load(load),
        .addr_h_w(addr_h_w), .data_h(data_h), .addr_h_r(addr_h_r), 
        .h(h)
      );

  // Mem_r
    mem_r #(.N(N), .q(q), .p(p)) 
      mem_r(
        .clk(clk), .load(load),
        .addr_r_w(addr_r_w), .data_r(data_r), .addr_r_r(addr_r_r), 
        .r(r)
      );

  // Mems_e
    mems_e #(.N(N), .q(q), .p(p), .M(M)) 
      mems_e(
        .clk(clk), .load(load), .operate(operate), .read(read),
        .addr_m_l(addr_m_l), .data_m(data_m), .addr_e(addr_e), .data_e(data_e), .addr_dout(addr_dout),
        .e(e), .eo(eo)
      );

  // clog2 function 
    function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction

endmodule

//
// Memory_r
//
module mem_r  # 
  (
    parameter N = 541,
    parameter q = 2048,
    parameter p = 3
  )( 
    input clk,
    input load,
 	input [clog2(N-1)-1:0] addr_r_w,
 	input [clog2(N-1)-1:0] addr_r_r,
    input [clog2(p-1)-1:0] data_r,    
	output wire [clog2(p-1)-1:0] r
  );

	reg [clog2(p-1)-1:0] Mem_r [N-1:0];
	reg [clog2(p-1)-1:0] r_reg;

	always @(posedge clk) 
	begin
		if (load)
			Mem_r[addr_r_w] <= data_r;
		r_reg <= Mem_r[addr_r_r];
	end
	assign r = r_reg;

  // clog2 function 
    function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction

endmodule

//
// Memories_h
//
module mems_h  # 
  (
    parameter N = 541,
    parameter q = 2048,
    parameter p = 3,
    parameter M = 1
  )( 
    input clk,
    input load,
 	input [clog2(N-1)-1:0] addr_h_w,
    input [clog2(N-1)-1:0] addr_h_r,
    input [clog2(q-1)-1:0] data_h,   
	output wire [M*clog2(q-1)-1:0] h
  );
 
	genvar i;
    generate
        for (i = 0; i < M; i = i + 1) begin : c
            wire [clog2(N-1):0] addr_h_wi;
            wire [clog2(N-1):0] addr_h_wn;
            assign addr_h_wi = addr_h_w + (N - i);
		end
		
        assign c[0].addr_h_wn = addr_h_w;
        
        for (i = 1; i < M; i = i + 1) begin
 	        assign c[i].addr_h_wn = (c[i].addr_h_wi < N) ? c[i].addr_h_wi: c[i].addr_h_wi-N;	
        end
      		
        for (i = 0; i < M; i = i + 1) begin : MEM_H                       
            mem_h #(.N(N), .q(q), .p(p)) 
              mem_h(
                .clk(clk), .load(load),
                .addr_h_w(c[i].addr_h_wn), .data_h(data_h), .addr_h_r(addr_h_r), 
                .h(h[(i+1)*clog2(q-1)-1:i*clog2(q-1)])
              );
        end             
	endgenerate
	
  // clog2 function 
    function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction

endmodule

module mem_h  # 
  (
    parameter N = 541,
    parameter q = 2048,
    parameter p = 3
  )( 
    input clk,
    input load,
 	input [clog2(N-1)-1:0] addr_h_w,
    input [clog2(N-1)-1:0] addr_h_r,
    input [clog2(q-1)-1:0] data_h,    
	output wire [clog2(q-1)-1:0] h
  );

	reg [clog2(q-1)-1:0] Mem_h [N-1:0];
	reg [clog2(q-1)-1:0] h_reg;

 	always @(posedge clk) 
	begin
        if (load)
			Mem_h[addr_h_w] <= data_h;
		h_reg <= Mem_h[addr_h_r];
	end
    assign h = h_reg ;
	
  // clog2 function 
    function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction

endmodule

//
// Memories_e
//
module mems_e  # 
  (
    parameter N = 541,
    parameter q = 2048,
    parameter p = 3,
    parameter M = 1
  )( 
    input clk,
    input load,
    input operate,
    input read,
    input [clog2($ceil(1.0*N/M)-1)+clog2(M-1)-1:0] addr_m_l,
    input [clog2($ceil(1.0*N/M)-1)+clog2(M-1)-1:0] addr_dout,
    input [clog2($ceil(1.0*N/M)-1)-1:0] addr_e,
    input [clog2(q-1)-1:0] data_m,
    input [M*clog2(q-1)-1:0] data_e,   
    output wire [clog2(q-1)-1:0] eo,
    output wire [M*clog2(q-1)-1:0] e
  );

    reg  [clog2($ceil(1.0*N/M)-1)-1:0] addr_e_d;
    wire [clog2($ceil(1.0*N/M)-1)-1:0] addr_e_r;
    wire [clog2($ceil(1.0*N/M)-1)-1:0] addr_e_w;
    wire [clog2($ceil(1.0*N/M)-1)-1:0] w_addr_m_l;
    wire [clog2($ceil(1.0*N/M)-1)-1:0] w_addr_dout;
    wire [clog2(M-1)-1:0] w_sel_e;
    wire [clog2(M-1)-1:0] r_sel_e;
    reg  [clog2(M-1)-1:0] r_sel_e_d;
    wire [M-1:0] w_cs;
    
    always @(posedge clk)
        addr_e_d <= addr_e;
        
    always @(posedge clk)
        r_sel_e_d <= r_sel_e;

	assign w_addr_m_l = addr_m_l[clog2($ceil(1.0*N/M)-1)+clog2(M-1)-1:clog2(M-1)];
	assign w_addr_dout = addr_dout[clog2($ceil(1.0*N/M)-1)+clog2(M-1)-1:clog2(M-1)];
    if (M > 1) 
    begin
        assign w_sel_e = addr_m_l[clog2(M-1)-1:0];
        assign r_sel_e = addr_dout[clog2(M-1)-1:0];
    end 
       
    assign addr_e_w = (load)  ? w_addr_m_l : addr_e_d;
    assign addr_e_r = (read) ? w_addr_dout : addr_e;
    	
	genvar i;
    generate
        for (i = 0; i < M; i = i + 1) begin : MEM_E
            mem_e #(.N(N), .q(q), .p(p), .M(M)) 
              mem_e(
                .clk(clk), .cs(w_cs[i]), .load(load), .operate(operate),
                .addr_e_w(addr_e_w), .addr_e_r(addr_e_r), .data_m(data_m), 
                .data_e(data_e[(i+1)*clog2(q-1)-1:i*clog2(q-1)]), 
                .e(e[(i+1)*clog2(q-1)-1:i*clog2(q-1)])
              );
              
            if (M == 1) begin : m1
                assign w_cs[0] = 1'b1; 
                assign eo = e;
            end
            else begin : m   
                assign w_cs[i] = (w_sel_e == i) ? 1'b1 : 1'b0;
                assign eo = (r_sel_e_d == i) ? e[(i+1)*clog2(q-1)-1:i*clog2(q-1)] : {clog2(q-1){1'bz}}; 
            end 
        end
    endgenerate

  // clog2 function 
    function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction

endmodule

// Mem_e

module mem_e  # 
  (
    parameter N = 541,
    parameter q = 2048,
    parameter p = 3,
    parameter M = 1
  )( 
    input clk,
    input cs,
    input load,
    input operate,
    input [clog2($ceil(1.0*N/M)-1)-1:0] addr_e_w,
    input [clog2($ceil(1.0*N/M)-1)-1:0] addr_e_r,
    input [clog2(q-1)-1:0] data_m,
    input [clog2(q-1)-1:0] data_e,   
    output wire [clog2(q-1)-1:0] e
  );

    localparam integer ne = ($ceil(1.0*N/M)-1);
    
 	reg  [clog2(q-1)-1:0] Mem_e [ne:0];  
 	reg  [clog2(q-1)-1:0] e_reg;	
 	wire [clog2(q-1)-1:0] din_e;
 
    assign din_e = (load) ? data_m : data_e;

	always @(posedge clk) 
	begin
		if ((load && cs) || operate)
			Mem_e[addr_e_w] <= din_e;
        e_reg <= Mem_e[addr_e_r];
	end
	
	assign e = e_reg;

  // clog2 function 
    function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction

endmodule


