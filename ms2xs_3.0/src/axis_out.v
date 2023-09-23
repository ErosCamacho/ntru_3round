//
// Verilog code for AXI4-Stream IP-Module of NTRU Serial Multiplier with M AUs 
// axis_out.v - AXI4-Stream output interface
// santiago@imse-cnm.csic.es (31/10/2020)
//

module axis_out # 
  (	
    parameter N  = 541,
	parameter M  = 1,
	parameter integer D_WIDTH = 16,
	parameter integer S_COUNT = 16,
	parameter [1:0] IDLE 		 = 2'b00,                                              
					INIT_COUNTER = 2'b01, 
					SEND_STREAM  = 2'b10
  )( 
	input  wire clk,
	input  wire reset,
	input  wire read,
	input  wire dout_tready,
	output wire dout_tvalid,
	output wire dout_tlast,
	output wire [clog2($ceil(1.0*N/M)-1)+clog2(M-1)-1:0] read_addr,
	output wire read_en,
	output wire read_done
  );

	reg  [1:0] mst_exec_state;
	reg  [clog2(S_COUNT-1)-1: 0] count;
	wire axis_tvalid;
	reg  axis_tvalid_delay;
	wire axis_tlast;
	reg  axis_tlast_delay;	
	wire tx_en;
	reg  tx_done;
	reg  tx_done_delay;	
	reg  [clog2(N-1)-1:0] read_pointer;
	reg  [clog2($ceil(1.0*N/M)-1)-1:0] addr_hi;
    reg  [clog2(M-1)-1:0] addr_lo;
	
	assign axis_tvalid  = ((mst_exec_state == SEND_STREAM) && (read_pointer < N) && read);
    assign axis_tlast   = (read_pointer == N-1);
	assign tx_en        = read && dout_tready && axis_tvalid;
	assign dout_tvalid	= axis_tvalid_delay;
    assign dout_tlast   = axis_tlast_delay;
    assign read_en      = tx_en;
    assign read_done    = tx_done_delay;
      	
	generate
	   if ( M == 1)
	       assign read_addr = read_pointer;
	   else
		  assign read_addr = {addr_hi, addr_lo};
    endgenerate
	
  // Control FSM
	always @(posedge clk)
	begin
		if (!reset)
			begin
				mst_exec_state <= IDLE;
				count <= {clog2(S_COUNT-1) {1'b0}};
			end
		else
			case (mst_exec_state)
			  IDLE:                      
				mst_exec_state <= INIT_COUNTER;
			  INIT_COUNTER:
				if ( count == S_COUNT - 1 )
					begin
						mst_exec_state <= SEND_STREAM;
					end
				else
					begin
						count <= count + 1;
						mst_exec_state <= INIT_COUNTER;
					end
			  SEND_STREAM:
				if (tx_done)
					begin
						mst_exec_state <= IDLE;
					end
				else
					begin
						mst_exec_state <= SEND_STREAM;
					end
			endcase
	end

  // Delay tvalid, tlast and done signals
	always @(posedge clk)  
	begin
        if (!reset)
			begin
                axis_tvalid_delay <= 1'b0;
                axis_tlast_delay <= 1'b0;
                tx_done_delay <= 1'b0;
            end
        else
            begin
				axis_tvalid_delay <= axis_tvalid;
				axis_tlast_delay <= axis_tlast;
	            tx_done_delay <= tx_done;
		end
	end
	
  // read_pointer
	always@(posedge clk)
	begin
		if(!reset)
			begin
				tx_done      <= 1'b0;
				read_pointer <= {clog2(N-1) {1'b0}};
				addr_lo      <= {clog2(M-1) {1'b0}};
				addr_hi      <= {clog2($ceil(1.0*N/M)-1) {1'b0}};
			end          
		else 
		if (read_pointer < N)
			begin
				if (tx_en)
					begin            
						read_pointer <= read_pointer + 1;
						tx_done <= 1'b0;
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
                if (read_pointer == N-1)
                    begin 
                        tx_done <= 1'b1;
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
