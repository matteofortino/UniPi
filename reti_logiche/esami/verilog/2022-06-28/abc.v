module ABC(
  rdx,
  ow,
  signal,
  out,
  clock, reset_
);
  input rdx, clock, reset_;
  output ow, signal;
  output[7:0] out;

  reg OW, SIGNAL;
  reg[7:0] OUT;

  assign ow = OW;
  assign signal = SIGNAL;
  assign out = OUT;


endmodule