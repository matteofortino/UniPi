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

reg OUT;
assign out = OUT;
reg RFD;
assign rfd_x = RFD;
assign rfd_y = RFD;


reg[7:0] COUNT;
wire[7:0] max; 
MAX m( .x(x), .y(y), .max(max));

reg[1:0] STAR;
localparam S0 = 0,
           S1 = 1,
           S2 = 2;

always @(reset_ == 0) begin
  OUT <= 0;  
  RFD <= 1;
  STAR <= S0;
end


always @(posedge clock) if(reset_ == 1) begin
  casex (STAR)
    S0: begin
      RFD <= 1;
      COUNT <= max;
      OUT <= 0;
      STAR <= ({dav_x, dav_y} == 2'b00) ? S1 : S0;
    end 
    S1: begin
      RFD <= 0; 
      OUT <= 0;
      STAR <= ({dav_x, dav_y} == 2'b11) ? S2 : S1;
    end
    S2: begin
      OUT <= 1;
      COUNT <= COUNT - 1;
      STAR <= (COUNT == 1) ? S0 : S2;
    end
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