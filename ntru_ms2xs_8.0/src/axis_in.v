//
// Verilog code for AXI4-Stream IP-Module of NTRU Serial Multiplier with M AUs 
// axis_in.v - AXI4-Stream input interface
// camacho@imse-cnm.csic.es (24/09/2023)
//

module axis_in #
 (  parameter N  = 541,
    parameter M  = 1,
	parameter integer D_WIDTH = 16,
    parameter [1:0]  IDLE  = 1'b0,         
                     WRITE = 1'b1
  )(
  	input wire  clk,
	input wire  reset,
	input wire  din_tvalid,
	input wire  din_tlast,
	output wire din_tready,
	output wire [clog2(N-1)-1:0] write_addr_h,
	output wire [clog2($ceil(1.0*N/M)-1)+clog2(M-1)-1:0] write_addr_m,
	output wire write_en,
 	output reg  write_done
  );
 
	reg  mst_exec_state;   
	wire axis_tready;
	reg  [clog2(N-1)-1:0] write_pointer; 
	reg  [clog2($ceil(1.0*N/M)-1)-1:0] addr_hi;
    reg  [clog2(M-1)-1:0] addr_lo;	
	
  	assign axis_tready   = ((mst_exec_state == WRITE) && (write_pointer <= N-1));
    assign din_tready    = axis_tready;
    assign write_en      = din_tvalid && axis_tready;
    assign write_addr_h  = write_pointer;

	generate
        if ( M == 1)
			assign write_addr_m = write_pointer;
        else
			assign write_addr_m = {addr_hi, addr_lo};
    endgenerate
		
  // Control FSM
	always @(posedge clk) 
	begin  
	  if (!reset) 
        begin
            mst_exec_state <= IDLE;
		end  
	  else
		case (mst_exec_state)
		  IDLE: 
			if (din_tvalid)
			  begin
                mst_exec_state <= WRITE;
			  end
			else
			  begin
                mst_exec_state <= IDLE;
			  end
		  WRITE: 
			if (write_done)
			  begin
                mst_exec_state <= IDLE;
			  end
			else
			  begin
                mst_exec_state <= WRITE;
		    end
		endcase
	end
	
  // write pointer
	always@(posedge clk)
	begin
      if(!reset)
        begin
          write_pointer <= {clog2(N-1) {1'b0}};
          write_done    <= 1'b0;
          addr_lo       <= {clog2(M-1) {1'b0}};
          addr_hi       <= {clog2($ceil(1.0*N/M)-1) {1'b0}};
        end  
      else
	  if (write_pointer < N)
        begin
          if (write_en)
            begin
              write_pointer <= write_pointer + 1;
              write_done <= 1'b0;
              if (addr_lo < M-1)
                begin
                  addr_lo <= addr_lo + 1;
                end
              else
                begin
                  addr_lo <= 1'b0;
                  addr_hi <= addr_hi + 1;
                end
            end
          if ((write_pointer == N-1) || din_tlast)
            begin
              write_done <= 1'b1;            
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