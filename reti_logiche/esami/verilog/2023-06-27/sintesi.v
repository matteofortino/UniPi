module ABC(
  rfd_x, dav_x, x,
  rfd_y, dav_y, y,
  out,
  clock, reset_
); 
input dav_x, dav_y;
input[7:0] x, y;
input clock, reset_;

output out;
output rfd_x, rfd_y;

PO po(
  rfd_x, dav_x, x,
  rfd_y, dav_y, y,
  out,
  clock, reset_,
  b0, b1,
  c0, c1, c2
);
PC pc(
  clock, reset_,
  b0, b1,
  c0, c1, c2
);
endmodule


module PO(
  rfd_x, dav_x, x,
  rfd_y, dav_y, y,
  out,
  clock, reset_,
  b0, b1,
  c0, c1, c2
);

input dav_x, dav_y;
input[7:0] x, y;
input clock, reset_;
input b0, b1;

output out;
output rfd_x, rfd_y;
output c0, c1, c2;

reg OUT;
assign out = OUT;
reg RFD;
assign rfd_x = RFD;
assign rfd_y = RFD;


reg[7:0] COUNT;
wire[7:0] max; 
MAX m( .x(x), .y(y), .max(max));

assign c0 = ~dav_x & ~dav_y; 
assign c1 = dav_x & dav_y;
assign c2 = (COUNT == 1);


always @(reset_ == 0) RFD <= 1;
always @(posedge clock) if(reset_ == 1) begin
  casex({b1,b0})
    2'b00: RFD <= 1;
    2'b01: RFD <= 0;
    2'b10: RFD <= RFD;
  endcase
end

always @(reset_ == 0) OUT <= 0;
always @(posedge clock) if(reset_ == 1) begin
  casex({b1,b0})
    2'b00: OUT <= 0;
    2'b01: OUT <= OUT;
    2'b10: OUT <=  1;
  endcase
end

always @(reset_ == 0) COUNT <= COUNT;
always @(posedge clock) if(reset_ == 1) begin
  casex({b1,b0})
    2'b00: COUNT <= max;
    2'b01: COUNT <= COUNT;
    2'b10: COUNT <= COUNT - 1;
  endcase
end
endmodule

module PC(
  clock, reset_,
  b0, b1,
  c0, c1, c2
);
input c0, c1, c2;
input clock, reset_;
output b0, b1;

assign {b1, b0} = (STAR == S0) ? 2'b00:
                  (STAR == S1) ? 2'b01 : 2'b10;

reg[1:0] STAR;
localparam S0 = 0,
           S1 = 1,
           S2 = 2;

always @(reset_ == 0) STAR <= S0;
always @(posedge clock) begin
  casex(STAR) 
    S0: STAR <= c0 ? S1 : S0;
    S1: STAR <= c1 ? S2 : S1;
    S2: STAR <= c2 ? S0 : S2;
  endcase
end

endmodule

module MAX(x, y, max);
  input[7:0] x, y;
  output[7:0] max; 

  wire c_out;
  add #(.N(8)) a (
    .x(x), .y(~y), .c_in(1'b1),
    .c_out(c_out)
  );
  assign max = c_out ? x : y;
endmodule