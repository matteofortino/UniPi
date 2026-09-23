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

  reg SOC;
  reg EOC_P;
  reg[7:0] X, Y;

  assign soc_vx = SOC;
  assign soc_vy = SOC;
  assign eoc_p = EOC_P; 
  assign x = X;
  assign y = Y;


  reg[2:0] STAR;
  localparam S0 = 0,
             S1 = 1,
             S2 = 2,
             S3 = 3,
             S4 = 4;

wire[7:0] x_new, y_new;
PROSSIMA_POSIZIONE pp (
  vx, vy,
  X, Y,
  x_new, y_new
);

always @(reset_ == 0) begin
  SOC <= 0;
  EOC_P <= 1; 
  STAR <= S0;
  X <= 0;
  Y <= 0;
end

always @(posedge clock) if(reset_ == 1) begin
  casex (STAR)
    S0: begin
      EOC_P <= 1;
      STAR <= soc_p == 1 ? S1 : S0; 
    end
    S1: begin
      EOC_P <= 0;
      STAR <= soc_p == 0 ? S2 : S1;
    end
    S2: begin
      SOC <= 1;
      STAR <= {eoc_vx, eoc_vy} == 2'b00 ? S3 : S2;
    end
    S3: begin
      SOC <= 0; 
      STAR <= {eoc_vx, eoc_vy} == 2'b11 ? S4 : S3;
    end
    S4: begin
      X <= x_new;
      Y <= y_new;
      STAR <= S0;
    end
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