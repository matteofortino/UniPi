module ABC(
  soc,
  eoc1, eoc2, eoc3,
  x1, x2, x3,
  dav_, rfd,
  min,
  clock, reset_
);
input eoc1, eoc2, eoc3;
input rfd;
input[7:0] x1, x2, x3;
input clock, reset_;

output soc;
reg SOC;
assign soc = SOC;

output dav_;
reg DAV_;
assign dav_ = DAV_;

output[7:0] min;
reg[7:0] MIN;
assign min = MIN;

reg[1:0] STAR;
localparam S0 = 0,
           S1 = 1,
           S2 = 2,
           S3 = 3; 



wire[7:0] minimo;
MINIMO_3 m3(
  x1, x2, x3,
  minimo
);

always @(reset_ == 0) begin
  SOC <= 0; 
  DAV_ <= 1;
  STAR <= S0;
end

always @(posedge clock) if(reset_ == 1) begin
  casex (STAR)
    S0: begin
      SOC <= 1; 
      STAR <= ({eoc1, eoc2, eoc3} == 0) ? S1 : S0;
    end
    S1: begin
      SOC <= 0;
      MIN <= minimo;
      STAR <= ({eoc1, eoc2, eoc3} == 3'b111) ? S2 : S1;
    end
    S2: begin
      DAV_ <= 0;
      STAR <= (rfd == 0) ? S3 : S2; 
    end
    S3: begin
      DAV_ <= 1;
      STAR <= (rfd == 1) ? S0 : S3;
    end
  endcase
end
endmodule


module MINIMO_3(
  x1, x2, x3,
  minimo
);
  input[7:0] x1, x2, x3;
  output[7:0] minimo;

  wire b_out1;
  sottrattore s1(
    .x(x1), .y(x2), .b_in(1'b0),
    .b_out(b_out1)
    );

  wire[7:0] min_x1_x2;
  assign min_x1_x2 = b_out1 ? x1 : x2;

  wire b_out2;
  sottrattore s2(
    .x(min_x1_x2), .y(x3), .b_in(1'b0),
    .b_out(b_out2)
  );

  assign minimo = b_out2 ? min_x1_x2 : x3;
endmodule
