//
// Verilog code for AXI4-Stream IP-Module of NTRU Serial Multiplier with M AUs 
// AUs.v - Arithmetic Units
// santiago@imse-cnm.csic.es (30/10/2020)
//

module aus # 
  (
    parameter N = 509,
    parameter q = 2048,
    parameter p = 3,
    parameter M = 1
  )( 
	input [clog2(p-1)-1:0] r,
 	input [M*clog2(q-1)-1:0]  h,
    input [M*clog2(q-1)-1:0]  e,   
	output [M*clog2(q-1)-1:0] e_out
  );

    wire [M*clog2(q-1)-1:0] e_out_11;
    //wire [M*clog2(q-1)-1:0] e_out_16;
    
    genvar i;
    generate
      for (i = 0; i < M; i = i + 1) begin:AU
//        au_11bits #(.N(N), .q(q), .qe(qe), .p(p)) 
//          au_11bits(
//            .clk(clk), .r(r), 
//            .h(h[(i+1)*clog2(qe-1)-1:i*clog2(qe-1)]), 
//            .e(e[(i+1)*clog2(qe-1)-1:i*clog2(qe-1)]), 
//            .e_out(e_out_11[(i+1)*clog2(qe-1)-1:i*clog2(qe-1)])
//          );
//        au_16bits #(.N(N), .q(q), .qe(qe), .p(p)) 
//          au_16bits(
//            .clk(clk), .r(r), 
//            .h(h[(i+1)*clog2(qe-1)-1:i*clog2(qe-1)]), 
//            .e(e[(i+1)*clog2(qe-1)-1:i*clog2(qe-1)]), 
//            .e_out(e_out_16[(i+1)*clog2(qe-1)-1:i*clog2(qe-1)])
//          );
         au_11bits_new #(.N(N), .q(q), .p(p)) 
          au_11bits_new(
            .r(r), 
            .h(h[(i+1)*clog2(q-1)-1:i*clog2(q-1)]), 
            .e(e[(i+1)*clog2(q-1)-1:i*clog2(q-1)]), 
            .e_out(e_out_11[(i+1)*clog2(q-1)-1:i*clog2(q-1)])
          );

         //always @* begin
            //if(h[(i+1)*clog2(qe-1)-1:(i+1)*clog2(qe-1)-6] == 0) e_out[(i+1)*clog2(qe-1)-1:i*clog2(qe-1)] = e_out_11[(i+1)*clog2(qe-1)-1:i*clog2(qe-1)];
            //else 
         assign e_out[(i+1)*clog2(q-1)-1:i*clog2(q-1)] = e_out_11[(i+1)*clog2(q-1)-1:i*clog2(q-1)];
         //end
      end
    endgenerate
		
  // clog2 function
    function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction
	
endmodule


//
// Module au_11bits_new
//
module au_11bits_new # 
  (
    parameter N = 541,
    parameter q = 2048,
    parameter p = 3
  )( 
	input [clog2(p-1)-1:0] r,
    input [clog2(q-1)-1:0] h,
	input [clog2(q-1)-1:0] e,
	output [clog2(q-1)-1:0] e_out
  );
    wire [clog2(q-1)-1:0] w_hxr;
        
    hxr #(.q(q)) hxr (.h(h[clog2(q-1)-1:0]), .r1(r[1]), .hxr(w_hxr));
    assign e_out = e + (w_hxr & {clog2(q-1){r[0]}});

  // clog2 function
   function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction

endmodule


//
// Module au_11bits
//
module au_11bits # 
  (
    parameter N = 541,
    parameter q = 2048,
    parameter qe = 65536,
    parameter p = 3
  )( 
    input clk,
	input [clog2(p-1)-1:0] r,
    input [clog2(qe-1)-1:0] h,
	input [clog2(qe-1)-1:0] e,
	output reg [clog2(qe-1)-1:0] e_out
  );
    wire [clog2(qe-1)-1:0] w_e_in;
    wire [clog2(q-1)-1:0] w_hxr;
    wire [4:0] w_rest;
    wire [4:0] w_out_rest;
    wire [clog2(qe-1)-1:0] h_desp;
    reg [clog2(qe-1)-1:0] w_acc;
    
    assign h_desp = h[clog2(q-1)-1:0] << clog2(q-1);
    
    hxr #(.q(q)) hxr (.h(h[clog2(q-1)-1:0]), .r1(r[1]), .hxr(w_hxr));
    rest rest_inst (.in(h_desp[(clog2(qe-1)-1):clog2(q-1)]), .out(w_rest));
  
    assign w_e_in = e; 
 
    always @(*)
    begin
        if (r == 0 || h == 0)
            e_out <= w_e_in;
        else begin
            if (r == 1) begin
                e_out <= w_e_in + {5'b00000,w_hxr};
                w_acc <= {5'b00000,w_hxr};
            end
            else begin
                e_out <= w_e_in + {w_rest,w_hxr};
                w_acc <= {w_rest,w_hxr};
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

//
// Module au_16bits
//
module au_16bits # 
  (
    parameter N = 541,
    parameter q = 2048,
    parameter qe = 65536,
    parameter p = 3
  )( 
    input clk,
	input [clog2(p-1)-1:0] r,
    input [clog2(qe-1)-1:0] h,
	input [clog2(qe-1)-1:0] e,
	output reg [clog2(qe-1)-1:0] e_out
  );
    wire [clog2(qe-1)-1:0] w_e_in;
    wire [clog2(qe-1)-1:0] w_hxr;
    wire [4:0] w_rest;
    wire [4:0] w_out_rest;
    wire [clog2(qe-1)-1:0] h_desp;
    reg [clog2(qe-1)-1:0] w_acc;
    
    assign h_desp = h << clog2(q-1);
    
    hxr #(.q(qe)) hxr (.h(h), .r1(r[1]), .hxr(w_hxr));
  
    assign w_e_in = e; 
 
    always @(*)
    begin
        if (r == 0 || h == 0)
            e_out <= w_e_in;
        else begin
            if (r == 1) begin
                e_out <= w_e_in + w_hxr;
                w_acc <= w_hxr;
            end
            else begin
                e_out <= w_e_in + (h_desp + w_hxr);
                w_acc <= w_hxr;
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

//
// Module REST
//
module rest (
    input [4:0] in,
    output [4:0] out
);
    assign out = in + 5'b11111;
    
endmodule

//
// Module hxr
//

module hxr #
  (
	parameter q = 2048
  )(  
	input [clog2(q-1)-1:0] h,
    input r1,
	output [clog2(q-1)-1:0] hxr
  );

    wire [clog2(q-1)-1:0] w_hxr;

    genvar i;
    generate
        for(i=0; i<clog2(q-1); i=i+1) begin : bit
            assign w_hxr[i] = h[i]^r1;
        end
        assign hxr = w_hxr + r1;
    endgenerate

  // clog2 function
    function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction

endmodule
