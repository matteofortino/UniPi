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
output dav_;
output[7:0] min;

parteOperativa PO(
  soc,
  eoc1, eoc2, eoc3,
  x1, x2, x3,
  dav_, rfd,
  min,
  clock, reset_,
  b0, b1, b2,
  c0, c1, c2, c3
);
parteControllo PC(
  b0, b1, b2,
  c0, c1, c2, c3,
  clock, reset_
);
endmodule


module parteOperativa (
  soc,
  eoc1, eoc2, eoc3,
  x1, x2, x3,
  dav_, rfd,
  min,
  clock, reset_,
  b0, b1, b2,
  c0, c1, c2, c3
);
  
input eoc1, eoc2, eoc3;
input rfd;
input[7:0] x1, x2, x3;
input clock, reset_;

output soc; 
output dav_;
output[7:0] min;


reg SOC, DAV_;
reg[7:0] MIN;
assign soc = SOC, dav_ = DAV_, min = MIN;

input b0, b1, b2;
output c0, c1, c2, c3;

assign c0 = ~eoc1 & ~eoc2 & ~eoc3;
assign c1 = eoc1 & eoc2 & eoc3;
assign c2 = ~rfd;
assign c3 = rfd;

always if(reset_ == 0) #1 begin
  SOC <= 0;
end
always @(posedge clock) if(reset_ == 1) #3 begin
  casex(b0)
    1'b0: SOC <= 0;
    1'b1: SOC <= 1;
  endcase
end

always if(reset_ == 0) #1 begin
  DAV_ <= 1;
end
always @(posedge clock) if(reset_ == 1) #3 begin
  casex(b1)
    1'b0: DAV_ <= 1;
    1'b1: DAV_ <= 0;
  endcase
end

wire[7:0] minimo;
MINIMO_3 m3(
  x1, x2, x3,
  minimo
);
always if(reset_ == 0) #1 begin
  MIN <= MIN;
end
always @(posedge clock) if(reset_ == 1) #3 begin
  casex(b2)
    1'b0: MIN <= MIN;
    1'b1: MIN <= minimo;
  endcase
end
endmodule

module parteControllo(
  b0, b1, b2,
  c0, c1, c2, c3,
  clock, reset_
);
input clock, reset_;

input c0, c1,  c2, c3;
output b0, b1, b2;


reg[1:0] STAR;
localparam S0 = 0,
           S1 = 1,
           S2 = 2,
           S3 = 3; 

assign {b0,b1,b2} = (STAR == S0) ? 3'b100 :
                    (STAR == S1) ? 3'b001 :
                    (STAR == S2) ? 3'b010 :
                    /* default */  3'b000;

always @(reset_ == 0) begin
  STAR <= S0; 
end
always @(posedge clock) if(reset_ == 1) begin
  casex(STAR)
    S0: STAR <= c0 ? S1 : S0;
    S1: STAR <= c1 ? S2 : S1;
    S2: STAR <= c2 ? S3 : S2;
    S3: STAR <= c3 ? S0 : S3;
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