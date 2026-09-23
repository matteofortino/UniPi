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

reg SOC;
assign soc = SOC;

reg DAV_;
assign dav_ = DAV_;

reg[31:0] Q;
assign q = Q;


reg[2:0] STAR;
localparam S0 = 0,
           S1 = 1,
           S2 = 2,
           S3 = 3;

wire[17:0] quadrato;
QUADRATO_DELLA_SOMMA qds (x, y, quadrato);

always @(reset_ == 0) begin
  SOC <= 0;  
  DAV_ <= 1;
  STAR <= S0;
end

always @(posedge clock) if(reset_ == 1) begin
  casex (STAR)
    S0: begin
      SOC <= 1;
      STAR <= {eocx, eocy} == 2'b00 ? S1 : S0;
    end 
    S1: begin
      SOC <= 0;
      Q <= quadrato;
      STAR <= {eocx, eocy} == 2'b11 ? S2 : S1;
    end
    S2: begin
      DAV_ <= 0;
      STAR <= rfd == 0 ? S3 : S2;
    end
    S3: begin
      DAV_ <= 1;
      STAR <= rfd == 1 ? S0 : S3;
    end
  endcase
end
endmodule


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