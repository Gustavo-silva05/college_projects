`timescale 1 ns/10 ps  // time-unit = 1 ns, precision = 10 ps

module tb;
  reg clock, reset, start;
  reg [31:0] A, B;
  wire end_mul;
  wire [63:0] produto;

  localparam PERIOD = 10;  

  multiplier DUT (.clock(clock), .reset(reset), .start(start), .A(A), .B(B), .end_mul(end_mul), .produto(produto));

  initial
  begin
    clock = 1'b0;
	forever #(PERIOD/2) clock = ~clock;
  end

  initial
  begin
    reset = 1'b1;
	#(PERIOD/2)
	reset = 1'b0;
  end

  initial
  begin
    A = 32'h12345678;
	B = 32'h33333333;
    #800
    A = 32'hFFFFFFFF;
	B = 32'hFFFFFFFF;
    #800
    A = 32'h00123045;
	B = 32'h33333333;
  end

  initial
  begin
    start = 1'b0;
    #30
    start = 1'b1;
    #(2*PERIOD)
    start = 1'b0;
	#800
    start = 1'b1;
    #(2*PERIOD)
    start = 1'b0;
	#800
    start = 1'b1;
    #(2*PERIOD)
    start = 1'b0;
  end

endmodule