module ABC(
  soc,
  eocx, x,
  eocy, y,
  dav_, rfd,
  q,
  clock, reset_
);
input clock, reset_;

input eocx, eocy, rfd;
input[7:0] x, y;

output soc, dav_;
output[31:0] q;

PO po(
  soc,
  eocx, x,
  eocy, y,
  dav_, rfd,
  q,
  clock, reset_,
  b0, b1, b2, b3,
  c0, c1, c2
);

PC pc(
  clock, reset_,
  b0, b1, b2, b3,
  c0, c1, c2
);
endmodule


module PO(
  soc,
  eocx, x,
  eocy, y,
  dav_, rfd,
  q,
  clock, reset_,
  b0, b1, b2, b3,
  c0, c1, c2
);
input clock, reset_;

input eocx, eocy, rfd;
input[7:0] x, y;

input b0, b1, b2, b3;
output c0, c1, c2;

output soc, dav_;
output[31:0] q;

reg SOC;
assign soc = SOC;

reg DAV_;
assign dav_ = DAV_;

reg[31:0] Q;
assign q = Q;


assign c0 = ~eocx & ~eocy;
assign c1 = eocx & eocy;
assign c2 = ~rfd;

wire[17:0] quadrato;
QUADRATO_DELLA_SOMMA qds (x, y, quadrato);

always @(reset_ == 0) SOC <= 0; 
always @(posedge clock) if(reset_ == 1) begin
  casex({b1,b0})
    2'b00: SOC <= 1;
    2'b01: SOC <= 0;
    2'b10: SOC <= SOC;
  endcase 
end
  
always @(reset_ == 0) DAV_ <= 1; 
always @(posedge clock) if(reset_ == 1) begin
  casex({b3,b2})
    2'b00: DAV_ <= DAV_;
    2'b01: DAV_ <= 0;
    2'b10: DAV_ <= 1;
  endcase 
end


always @(reset_ == 0) Q <= Q; 
always @(posedge clock) if(reset_ == 1) begin
  casex(b0)
    1'b0: Q <= Q;
    1'b1: Q <= quadrato;
  endcase 
end
endmodule


module PC(
  clock, reset_,
  b0, b1, b2, b3,
  c0, c1, c2
);
input clock, reset_;
input c0, c1, c2;

output b0, b1, b2, b3;

assign {b3,b2,b1,b0} = STAR == S0 ? 4'b0000 :
                       STAR == S1 ? 4'b0001 :
                       STAR == S2 ? 4'b0110 : 4'b1010;


reg[2:0] STAR;
localparam S0 = 0,
           S1 = 1,
           S2 = 2,
           S3 = 3;

always @(reset_ == 0) STAR <= S0;
always @(posedge clock) if(reset_ == 1) begin
  casex (STAR)
    S0: STAR <= c0 ? S1 : S0; 
    S1: STAR <= c1 ? S2 : S1; 
    S2: STAR <= c2 ? S3 : S2; 
    S3: STAR <= ~c2 ? S0 : S3; 
  endcase
end
  
endmodule;

module QUADRATO_DELLA_SOMMA(
  x, y,
  quadrato
);

input[7:0] x, y;
output[17:0] quadrato;

wire[15:0] x_quadrato;
mul_add_nat m1 (
  .x(x), .y(x), .c(8'b0),
  .m(x_quadrato)
);

wire[15:0] y_quadrato;
mul_add_nat m2 (
  .x(y), .y(y), .c(8'b0),
  .m(y_quadrato)
);

wire[15:0] xy;
mul_add_nat m3 (
  .x(x), .y(y), .c(8'b0),
  .m(xy)
);

wire[16:0] xy2;
assign xy2 = {xy[15:0], 1'b0};


wire[16:0] s1;
add #(.N(16)) a1 (
  .x(x_quadrato), .y(y_quadrato), .c_in(1'b0),
  .c_out(s1[16]), .s(s1[15:0]) 
);


add #(.N(17)) a2 (
  .x(s1), .y(xy2), .c_in(1'b0),
  .c_out(quadrato[17]), .s(quadrato[16:0]) 
);


endmodule