//
// Verilog code for AXI4-Stream IP-Module of NTRU Serial Multiplier with M AUs 
// Control.v - control signals genetration
// santiago@imse-cnm.csic.es (30/10/2020)
//

module control #
  (
    parameter N = 541,
    parameter q = 2048,
    parameter p = 3,
    parameter M = 1,
    parameter max_cycles = 400
  )(
    input clk,
    input rst,
    input start_op,
    input [clog2(p-1)-1:0] r,
    input [clog2(N)-1:0] nnz,
    output [clog2(N-1)-1:0] addr_h,
    output [clog2(N-1)-1:0] addr_r,
    output [clog2($ceil(1.0*N/M)-1)-1:0] addr_e,
    output operate,
    output end_op
  );

    wire [clog2(N-1):0]   i;                        // OK
    reg  [clog2(N-1)-1:0] j;                        // = {clog2(N-1){1'b0}};
    reg  [clog2(N-1)-1:0] kn;                       // = {clog2(N-1){1'b0}};
    reg  [clog2($ceil(1.0*N/M)-1)-1:0] k;           // = {clog2($ceil(1.0*N/M)-1){1'b0}};
    reg  [clog2(N*N)-1:0] nc;
    reg  [clog2(N)-1:0] nz;
    reg  reg_oper;
    reg  reg_end_op;
    
    wire end_k;
    wire [clog2(N)-1:0] nz_max;
    
    localparam integer k1 = $ceil(1.0*N/M)-1;
    localparam integer ncc = max_cycles*($ceil(1.0*N/M)) + (N - max_cycles);
    
    assign end_k = (M == 1) ? (k == N-1) : (kn == (N/M)*M);
    assign nz_max = max_cycles - nnz;
 //   
 // Counter j
 //
 	always @(negedge clk) 
    begin
        if (!rst) 
            j <= {clog2(N-1){1'b0}};
        else 
            if (j < N-1 && nz < nz_max) // Estos ciclos hay que hacerlos
                if (operate && end_k)
                    j <= j + 1'b1;
                else
                    j <= j;
            else if (j < N-1 && nz >= nz_max) // Ya he contado el maximo de ceros y ya puedo salta
                if (operate && (r == 0 || end_k))
                    j <= j + 1'b1;
                else
                    j <= j;
    end

//
// Counter k   
//
 	always @(negedge clk) 
    begin
        if (!rst) k <= {clog2(N-1){1'b0}};
        else begin
            if (k < k1) begin       //            if (k < $ceil(1.0*N/M)-1)
                if (nz < nz_max) begin // Este tengo que hacerlo porque quiero llegar al max_cycle
                    if (operate)
                        k <= k + 1'b1;
                    else
                        k <= k;
                end
                else if (nz >= nz_max) begin // Este ya me puedo saltar
                    if (operate && r != 0)
                        k <= k + 1'b1;
                    else
                        k <= k;               
                end
            end
            else k <= {clog2(N-1){1'b0}};
         end
    end

    generate 
        if (M == 1)
            assign i = N-j+k;
        else
          begin 
            //
            // Counter kn  
            // 
            always @(negedge clk) 
                begin
                    if (!rst) 
                        kn <= {clog2(N-1){1'b0}};
                    else 
                        if (kn < ((N/M)-1)*M+1) begin       //            if (k < $ceil(1.0*N/M)-1)
                            if (nz < nz_max) begin // Este tengo que hacerlo porque quiero llegar al max_cycle
                                if (operate) kn <= kn + M;
                                else kn <= kn;
                            end
                            else if (nz >= nz_max) begin // Este ya me puedo saltar
                                if (operate && r != 0) kn <= kn + M;
                                else kn <= kn;               
                            end
                        end
                        else 
                            kn <= {clog2(N-1){1'b0}};
                end
  
                assign i = N-j+kn;
          end        
    endgenerate
  
//
// Counter nc   
//
    always @(posedge clk) 
    begin
        if (!rst || (nc == ncc-1)) 
            nc <= {clog2(N*N){1'b0}};
        else
            if (reg_oper) 
                nc <= nc + 1'b1;
    end
  
  // 
 // Counter nz -> Contador numeros igual a 0 en r DURANTE LA OPERACION
 //
    always @(posedge clk) 
    begin
        if (!rst) nz <= {clog2(N){1'b0}};
        
        if (operate) begin
            if (r == 0 && k == 0) nz <= nz + 1'b1; // Para que cuente solo el primer 0
            else nz <= nz;
        end
    end

//
// Operate   
//
    always @(posedge clk) 
    begin
        if (!rst)
            begin 
                reg_oper <= 1'b0;
                reg_end_op <= 1'b0;
            end
        else
            if (start_op && !end_op) 
                    reg_oper <= 1'b1;
            if (nc == ncc-1)
                begin
                    //nc <= {clog2(2*dr*N+N-2*dr){1'b0}};
                    reg_oper <= 1'b0;
                    reg_end_op <= 1'b1;
               end
    end       

    assign addr_h = (i > N-1) ? i-N : i;
    assign addr_r = j;
    assign addr_e = k;
    assign operate = reg_oper;
    assign end_op = reg_end_op;

    
  // clog2 function
    function integer clog2;
      input integer n;
        for (clog2=0; n>0; clog2=clog2+1)
          n = n >> 1;
    endfunction
          
endmodule
