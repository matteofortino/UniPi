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

reg SOC, Z;
assign soc = SOC;
assign z = Z;

reg[7:0] M;
assign m = M;


reg[1:0] STAR;
localparam S0 = 0,
           S1 = 1,
           S2 = 2;


reg[7:0] X;
wire[7:0] media;
MEDIA_ESPONENZIALE me (
  .m(M), .x_next(X),
  .m_next(media)
);
always @(reset_ == 0) begin
  Z <= 0; 
  M <= 0;
  SOC <= 0;
end

always @(posedge clock) if(reset_ == 1) begin
  casex (STAR)
    S0: begin
      Z <= 0;
      SOC <= 1; 
      STAR <= eoc == 0 ? S1 : S0; 
    end 
    S1: begin
      X <= x;
      SOC <= 0;
      STAR <= eoc == 1 ? S2 : S1;
    end
    S2: begin
      M <= media;
      Z <= 1;
      STAR <= S0;
    end
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