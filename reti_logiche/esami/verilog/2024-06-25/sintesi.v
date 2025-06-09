module ABC(
  soc_vx, eoc_vx, vx,
  soc_vy, eoc_vy, vy,
  x, y,
  soc_p, eoc_p,
  clock, reset_
);
  input clock, reset_;
  input eoc_vx, eoc_vy;
  input[3:0] vx, vy;
  input soc_p;

  output soc_vx, soc_vy;
  output eoc_p;
  output[7:0] x, y;

PO po(
  soc_vx, eoc_vx, vx,
  soc_vy, eoc_vy, vy,
  x, y,
  soc_p, eoc_p,
  clock, reset_,
  b4, b3, b2, b1, b0,
  c0, c1, c2
);

PC pc(
  clock, reset_,
  b4, b3, b2, b1, b0,
  c0, c1, c2
);
endmodule


module PO (
  soc_vx, eoc_vx, vx,
  soc_vy, eoc_vy, vy,
  x, y,
  soc_p, eoc_p,
  clock, reset_,
  b4, b3, b2, b1, b0,
  c0, c1, c2
);
  input clock, reset_;
  input eoc_vx, eoc_vy;
  input[3:0] vx, vy;
  input soc_p;

  output soc_vx, soc_vy;
  output eoc_p;
  output[7:0] x, y;

  reg SOC;
  reg EOC_P;
  reg[7:0] X, Y;

  assign soc_vx = SOC;
  assign soc_vy = SOC;
  assign eoc_p = EOC_P; 
  assign x = X;
  assign y = Y;

input b4, b3, b2, b1, b0;
output c2, c1, c0;


assign c0 = soc_p;
assign c1 = ~eoc_vx & ~eoc_vy;
assign c2 = eoc_vx & eoc_vy;

wire[7:0] x_new, y_new;
PROSSIMA_POSIZIONE pp (
  vx, vy,
  X, Y,
  x_new, y_new
);

always @(reset_ == 0) SOC <= 0;
always @(posedge clock) if(reset_ == 1) begin
  casex({b1,b0})
    2'b00: SOC <= SOC;
    2'b01: SOC <= 1;
    2'b10: SOC <= 0;
  endcase
end

always @(reset_ == 0) EOC_P <= 1;
always @(posedge clock) if(reset_ == 1) begin
  casex({b3,b2})
    2'b00: EOC_P <= 1;
    2'b01: EOC_P <= 0;
    2'b10: EOC_P <= EOC_P;
  endcase
end

always @(reset_ == 0) X <= 0;
always @(posedge clock) if(reset_ == 1) begin
  casex(b4)
    1'b0: X <= X;
    1'b1: X <= x_new;
  endcase
end

always @(reset_ == 0) Y <= 0;
always @(posedge clock) if(reset_ == 1) begin
  casex(b4)
    1'b0: Y <= Y;
    1'b1: Y <= y_new;
  endcase
end
endmodule


module PC(
  clock, reset_,
  b4, b3, b2, b1, b0,
  c0, c1, c2
);
input clock, reset_;
input c0, c1, c2;
output b4, b3, b2, b1, b0;


assign {b4, b3, b2, b1, b0} = STAR == S0 ? 5'b00000 :
                              STAR == S1 ? 5'b00100 :
                              STAR == S2 ? 5'b01001 :
                              STAR == S3 ? 5'b01010 : 5'b11000;

reg[2:0] STAR;
localparam S0 = 0,
           S1 = 1,
           S2 = 2,
           S3 = 3,
           S4 = 4;

always @(reset_ == 0) STAR <= S0;
always @(posedge clock) if(reset_ == 1) begin
  casex(STAR)
    S0: STAR <= c0 ? S1 : S0; 
    S1: STAR <= ~c0 ? S2 : S1; 
    S2: STAR <= c1 ? S3 : S2; 
    S3: STAR <= c2 ? S4 : S3; 
    S4: STAR <= S0; 
  endcase
end
endmodule

module PROSSIMA_POSIZIONE(
  vx, vy,
  x, y,
  x_new, y_new
);
  input[3:0] vx, vy;
  input[7:0] x, y;
  output[7:0] x_new, y_new;

  wire[7:0] test_x, test_y;

  wire owx, owy;


  wire[7:0] vx_ext = {vx[3], vx[3], vx[3], vx[3], vx};

  wire[7:0] vy_ext = {vy[3], vy[3], vy[3], vy[3], vy};


  add #(.N(8)) a1 (
    .x(x), .y(vx_ext), .c_in(1'b0),
    .s(test_x), .ow(owx)
  );

  add #(.N(8)) a2 (
    .x(y), .y(vy_ext), .c_in(1'b0),
    .s(test_y), .ow(owy)
  );

  assign x_new = owx ? x : test_x;
  assign y_new = owy ? y : test_y;


endmodule


// ROM
/*
  | u_addr | b3b2b1b0 | c_eff | u_addr_true | u_addr_false |
  |  00    |   0000   |   00  |     01      |      00      |
  |  01    |   0001   |   01  |     10      |      01      |
  |  10    |   0110   |   10  |     11      |      10      |
  |  11    |   1010   |   10  |     00      |      11      |
*/