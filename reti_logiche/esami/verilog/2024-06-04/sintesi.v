module ABC(
  soc, eoc, x,
  m, z,
  clock, reset_
);
input clock, reset_;
input eoc;
input[7:0] x;

output soc, z;
output[7:0] m;

PO po(
  soc, eoc, x,
  m, z,
  clock, reset_,
  b1, b0,
  c0,
);

PC pc(
  clock, reset_,
  b1, b0,
  c0,
);
endmodule


module PO(
  soc, eoc, x,
  m, z,
  clock, reset_,
  b1, b0,
  c0,
);
input clock, reset_;
input eoc;
input[7:0] x;
input b0,b1;

output soc, z;
output c0;
output[7:0] m;

reg SOC, Z;
assign soc = SOC;
assign z = Z;

reg[7:0] M;
assign m = M;

reg[7:0] X;

wire[7:0] media;
MEDIA_ESPONENZIALE me (
  .m(M), .x_next(X),
  .m_next(media)
);

assign c0 = ~eoc;

always @(reset_ == 0) SOC <= 0;
always @(posedge clock) if(reset_ == 1) begin
  casex({b1,b0})
    2'b00: SOC <= 1;
    2'b01: SOC <= 0;
    2'b10: SOC <= SOC;
  endcase
end
  
always @(reset_ == 0) Z <= 0;
always @(posedge clock) if(reset_ == 1) begin
  casex({b1,b0})
    2'b00: Z <= 0;
    2'b01: Z <= Z;
    2'b10: Z <= 1;
  endcase
end

always @(reset_ == 0) M <= 0;
always @(posedge clock) if(reset_ == 1) begin
  casex(b1)
    1'b0: M <= M;
    1'b1: M <= media;
  endcase
end

always @(reset_ == 0) X <= X;
always @(posedge clock) if(reset_ == 1) begin
  casex(b0)
    1'b0: X <= X;
    1'b1: X <= x;
  endcase
end
endmodule


module PC (
  clock, reset_,
  b1, b0,
  c0,
);
input clock, reset_;
input c0;
output b0, b1;


reg[1:0] STAR;
localparam S0 = 0,
           S1 = 1,
           S2 = 2;
assign {b1,b0} = STAR == S0 ? 2'b00 :
                 STAR == S1 ? 2'b01 : 2'b10; 

always @(reset_ == 0) STAR <= S0;
always @(posedge clock) if(reset_ == 1) begin
  casex(STAR)
    S0: STAR <= c0 ? S1 : S0;
    S1: STAR <= ~c0 ? S2 : S1;
    S2: STAR <= S0;
  endcase
end

endmodule

module MEDIA_ESPONENZIALE(
  m, x_next,
  m_next
);
input[7:0] m, x_next;
output[7:0] m_next;

wire[9:0] mx3;
mul_add_nat #(.N(8), .M(2)) m1 (
  .x(m), .y(2'd3), .c(8'b0),
  .m(mx3)
);
wire[7:0] primo_termine;
assign primo_termine = mx3[9:2];

add #(.N(8)) a (
  .x(primo_termine), .y({2'b00, x_next[7:2]}), .c_in(1'b0),
  .s(m_next)
);

endmodule