// Final_JH_YM
//	Codes for SPI to receive color codes, Decoder to break up color codes for each LED strip
// and FSM to light up and blink an LED

// Testing module for one LED light
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

// Test module for receiving codes and blinking LEDs
// ToDO: Integrate with SPI and decoder
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

module SPI_LED_test_testbench();
	logic clk;
	logic sck;
	logic sdi;
	logic ce;
	logic [23:0] colorbits;
	logic led;
	logic [8:0] i;	
	
	SPI_LED_test dut(clk, sck, sdi, ce, led);
	
	initial begin
		colorbits  <= 24'h12345123451234512345123451234512345;
	end 
	
	    // generate clock and ce signals
	initial 
	  forever begin
			clk = 1'b0; #5;
			clk = 1'b1; #5;
	  end
	  
	initial begin
	sck = 0;
	i = 0;
	ce = 1'b1;
	end 

	// shift in test vectors, wait until done, and shift out result
	always @(posedge clk) begin
	if (i == 24) begin
		ce = 1'b0;
		i = i + 1;
	end if (i<24) begin
	  #1; sdi = colorbits[23-i];
	  #1; sck = 1; #5; sck = 0;
	  i = i + 1;
	end else i = i + 1;
	end

endmodule

// Test for both SPI and LED FSM
module SPI_LED_test(input  logic clk,
					 input  logic sck, 
                input  logic sdi,
                input  logic ce,
					 output logic led);
	logic fclk;
	logic loadr, nextloadr;
	logic [23:0] colorbits;
	logic ce_prev;
	logic [7:0] counter, nextcounter;
	
	PLL p(clk, fclk);	
	spi s(sck, sdi, ce, colorbits);
	FPGA_ledControlUnit f(colorbits, loadr, fclk, led);	
	
	always@(posedge clk) begin
		ce_prev <= ce;
		loadr <= nextloadr;
		counter <= nextcounter;
//		if (ce_prev & (~ce)) begin // If ce bit changes to low, start loading
//			loadr <= 1;
//			counter <= 0;
//		end
//		else if ((loadr) & (counter < 100)) begin // Leave 100 clock cycles for load
//			counter <= counter + 1;
//		end
//		else if ((loadr) & (counter >= 100)) begin
//			counter <= 0;
//			loadr <= 0;
//		end
	end

	always_comb begin
		if (ce_prev & (~ce)) begin
			nextloadr <= 1;
			nextcounter <= 0;
		end
		else if ((loadr) & (counter < 100)) begin
			nextloadr <= loadr;
			nextcounter <= counter + 1;
		end
		else if ((loadr) & (counter >= 100)) begin
			nextloadr <= 0;
			nextcounter <= 0;			
		end
		else begin
			nextloadr <= 0;
			nextcounter <= 0;			
		end
	end
	
	

endmodule
	 


// LED test bench for generating the correct waveforms for one LED strip
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

// module for lighting up one LED strip.
module LED_test(input  logic load,
					 input  logic clk,
					 output logic led,
					 output logic manualLoad);
	
	logic fclk;
	logic [23:0] colorbits;
	logic [25:0] counter;
	logic dummyload;
	PLL p(clk, fclk);
//	assign fclk = clk; // for testing
	FPGA_ledControlUnit f(colorbits, dummyload, fclk, led);
	
	// for debugging
//	assign manualLoad = 1;
//
//	always@(posedge clk)
//		if (counter < 12000000) begin
//			counter <= counter + 1;
//			dummyload <= 1;
//			colorbits <= 24'hff00e0000f0000ff00ff00e000f00f00ff0000ff00;
//		end
//		else if (counter < 24000000) begin 
//			dummyload = 0;
//			counter <= counter + 1;
//		end
//		else if (counter < 36000000) begin
//			dummyload <= 1;
//			counter <= counter + 1;
//			colorbits = 24'h00ffa0ff0000a000ffff00e00000ff00ff0000ff00;
//		end
//		else if (counter < 48000000) begin
//			dummyload <= 0;
//			counter <= counter + 1;
//		end
//		else counter <= 0;
endmodule

// Testbench for the nextstate logic of the FSM
module FPGA_nextstate_testbench();
	logic [23:0] colorbits;
	logic load;
	logic fclk;
	logic led;
	
	FPGA_ledControlUnit DUT(colorbits, load, fclk, led);
	
	 initial begin   
	// Test case 
//        colorbits  <= 24'h010101010101010101010101010101010101010101;
//		  expected   <= 24'h010101010101010101010101010101010101010101;
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

// generating the correct waveforms for one LED strip
module FPGA_ledControlUnit(input  logic [23:0] colorbits,
									input  logic load,
									input  logic fclk,
									output logic led);
									
	logic [2:0] state, nextstate;
	logic [6:0] counter, nextcounter;
	logic [23:0] bits;
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
				if (load) nextstate = 3'b001;
				else nextstate = 3'b000;	
			end
			3'b001: begin
				if (load) nextstate = 3'b001;
				else nextstate = 3'b010;
			end		
			3'b010: begin
				if (numBits >= 24) nextstate = 3'b101;
				else if (bits[23 - numBits] == 0) nextstate = 3'b011;
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

// Testbench for the shift registers of the SPI
module spi_testbench();
    logic clk, ce, sck, sdi, sdo;
    logic [23:0] colorbits, colorbits_out, expected;
    logic [8:0] i;
    
    // device under test
    spi dut(sck, sdi, ce, colorbits_out);
    
    // test case
    initial begin   
	// Test case 
//        colorbits  <= 24'h010101010101010101010101010101010101010101;
//		  expected   <= 24'h010101010101010101010101010101010101010101;
	// Alternate test case
   colorbits  <= 24'h10101010101f101010103010101010101010101010;
   expected  <= 24'h10101010101f101010103010101010101010101010;
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
      if (i == 24) begin
			ce = 1'b0;
			i = i + 1;
      end if (i<24) begin
        #1; sdi = colorbits[23-i];
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

// SPI module for receiving color codes
module spi(input  logic sck, 
               input  logic sdi,
               input  logic ce,
					output logic [23:0] colorbits);
    always_ff @(posedge sck)
        if (ce)  colorbits = {colorbits[22:0], sdi};
endmodule
