module blink_test(input logic clk,
						output logic blink,
						output logic led,
						output logic [6:0] uled);

	logic [25:0] counter;
	logic [8:0] numLEDs;
	logic fclk;
	logic [5:0] bits;
	PLL p(clk, fclk);
	
	always@(posedge fclk) begin
		if (numLEDs > 23) begin
			blink <= 0;
		end
		else if (counter < 80) begin
			led <= ~led;
			blink <= 1;
			counter <= counter + 1;	
		end
		else if (counter < 125) begin
			blink <= 0;
			counter <= counter + 1;
		end
		else begin
			counter <= 0;
			numLEDs <= numLEDs + 1;
		end
	end

	assign uled = 7'b0000001;
endmodule

module spi_blink_test(input  logic sck, 
						input  logic sdi,
						input  logic ce,
						input logic clk,
						output logic blink,
						output logic led,
						output logic [6:0] uled);

	logic [25:0] counter;
	logic [8:0] numLEDs;
	logic fclk;
	logic [5:0] bits;
	PLL p(clk, fclk);
	
	always@(posedge fclk) begin
		if (numLEDs > 23) begin
			blink <= 0;
		end
		else if (counter < 80) begin
			led <= ~led;
			blink <= 1;
			counter <= counter + 1;	
		end
		else if (counter < 125) begin
			blink <= 0;
			counter <= counter + 1;
		end
		else begin
			counter <= 0;
			numLEDs <= numLEDs + 1;
		end
	end

	assign uled = 7'b0000001;
endmodule

module LED_test_testbench();
	logic load;
	logic clk;
	logic led;
	
	LED_test dut(load, clk, led);
	
	initial begin
		load = 1;
		#20;
		load = 0;
	end

	initial
	forever begin
      clk = 1'b0; #5;
      clk = 1'b1; #5;			
	end
endmodule

module LED_test(input  logic load,
					 input  logic clk,
					 output logic led,
					 output logic manualLoad);
	
	logic fclk;
	logic [47:0] colorbits;
	logic [20:0] counter;

	PLL p(clk, fclk);
//	assign fclk = clk; // for testing
	assign colorbits = 48'h0000006789ab;
	FPGA_ledControlUnit f(colorbits, load, fclk, led);
	
	// for debugging
	assign manualLoad = 1;
	logic dummyload;
	always@(posedge clk)
		if (counter < 10000000) begin
			counter <= counter + 1;
			dummyload = 1;
		end
		else begin 
			dummyload = 0;
		end
endmodule

module FPGA_nextstate_testbench();
	logic [47:0] colorbits;
	logic load;
	logic fclk;
	logic led;
	
	FPGA_ledControlUnit DUT(colorbits, load, fclk, led);
	
	 initial begin   
	// Test case 
//        colorbits  <= 168'h010101010101010101010101010101010101010101;
//		  expected   <= 168'h010101010101010101010101010101010101010101;
	// Alternate test case
		colorbits  <= 48'h8123456789ab;
		load  <= 1;
		#30;
		load  <= 0;
    end
    
    // generate clock and ce signals
    initial 
        forever begin
            fclk = 1'b0; #5;
            fclk = 1'b1; #5;
        end

endmodule


module FPGA_ledControlUnit(input  logic [47:0] colorbits,
									input  logic load,
									input  logic fclk,
									output logic led);
									
	logic [2:0] state, nextstate;
	logic [6:0] counter, nextcounter;
	logic [47:0] bits;
	logic [7:0] numBits, nextnumBits;
	
	always@(posedge fclk)
		begin
			state   <= nextstate;
			counter <= nextcounter;
			numBits <= nextnumBits;
		end

		
	always_comb begin
		case(state)
			3'b000: begin
				if (~load) nextstate = 3'b000;
				else nextstate = 3'b001;	
			end
			3'b001: begin
				if (~load) nextstate = 3'b010;
				else nextstate = 3'b001;
			end		
			3'b010: begin
				if (numBits >= 48) nextstate = 3'b101;
				else if (bits[47 - numBits] == 0) nextstate = 3'b011;
				else                         nextstate = 3'b100;
			end		
			3'b011: begin
				if (counter < 125) nextstate = 3'b011;
				else               nextstate = 3'b010;
			end		
			3'b100: begin
				if (counter < 125) nextstate = 3'b100;
				else               nextstate = 3'b010;
			end
			3'b101: begin
				if (counter < 125) nextstate = 3'b000;
				else               nextstate = 3'b001;
			end
			default: nextstate = 3'b000;
		endcase
	end
	
	always_comb begin
		case(state)
			3'b000: begin
				led = 0;
				nextcounter = 0;
				nextnumBits = 0;
				bits = 0;
			end
			3'b001: begin
				led = 0;
				nextcounter = 0;
				nextnumBits = 0;
				bits = colorbits;
			end
			3'b010: begin
				led = 0;
				nextnumBits = numBits + 1;
				nextcounter = 0;
				bits = colorbits;
			end
			3'b011: begin
				if (counter < 40) led = 1;
				else              led = 0;
				nextcounter = counter + 1;
				nextnumBits = numBits;
				bits = colorbits;
			end
			3'b100: begin
				if (counter < 80) led = 1;
				else              led = 0;
				nextcounter = counter + 1;
				nextnumBits = numBits;
				bits = colorbits;
			end
			3'b101: begin
				led = 0;
				nextcounter = counter + 1;
				nextnumBits = 0;
				bits = colorbits;
			end						
			default: begin
				led = 0;
				nextcounter = 0;
				nextnumBits = 0;
				bits        = 0;
			end
		endcase
	end
endmodule


module spi_testbench();
    logic clk, ce, sck, sdi, sdo;
    logic [167:0] colorbits, colorbits_out, expected;
    logic [8:0] i;
    
    // device under test
    spi dut(sck, sdi, ce, colorbits_out);
    
    // test case
    initial begin   
	// Test case 
//        colorbits  <= 168'h010101010101010101010101010101010101010101;
//		  expected   <= 168'h010101010101010101010101010101010101010101;
	// Alternate test case
   colorbits  <= 168'h10101010101f101010103010101010101010101010;
   expected  <= 168'h10101010101f101010103010101010101010101010;
    end
    
    // generate clock and ce signals
    initial 
        forever begin
            clk = 1'b0; #5;
            clk = 1'b1; #5;
        end
        
    initial begin
      i = 0;
      ce = 1'b1;
		end 
    
    // shift in test vectors, wait until done, and shift out result
    always @(posedge clk) begin
      if (i == 168) begin
			ce = 1'b0;
			i = i + 1;
      end if (i<168) begin
        #1; sdi = colorbits[167-i];
        #1; sck = 1; #5; sck = 0;
        i = i + 1;
      end else if (i == 200) begin
            if (colorbits_out == expected)
                $display("Testbench ran successfully");
            else $display("Error: colorbits_out = %h, expected %h",
                colorbits_out, expected);
            $stop();
      end else i = i + 1;
    end
    
endmodule

module spi(input  logic sck, 
               input  logic sdi,
               input  logic ce,
					output logic [23:0] colorbits);
    always_ff @(posedge sck)
        if (ce)  colorbits = {colorbits[22:0], sdi};
endmodule
