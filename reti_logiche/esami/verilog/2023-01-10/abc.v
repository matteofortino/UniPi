module ABC(
  soc, eoc, x,
  rfd1, rfd2, rfd3, 
  dav1_, dav2_, dav3_,
  c1, c2, c3,
  clock, reset_,
);
  input reset_, clock;

  input eoc;
  input rfd1, rfd2, rfd3;
  input[7:0] x;

  output soc;
  output dav1_, dav2_, dav3_;
  output[2:0] c1, c2, c3;

  reg SOC; 
  assign soc = SOC;
  reg DAV_;
  assign dav1_ = DAV_;
  assign dav2_ = DAV_;
  assign dav3_ = DAV_;

  reg[2:0] C;
  assign c1 = C; 
  assign c2 = C; 
  assign c3 = C; 

  reg[7:0] X;

  reg[2:0] STAR;
  localparam S0 = 0,
             S1 = 1,
             S2 = 2,
             S3 = 3,
             S4 = 4;

  always @(reset_ == 0) begin
    STAR <= S0;
    DAV_ <= 1;
    SOC <= 0;
  end


  always @(posedge clock) if (reset_ == 1) begin
    casex (STAR)
      S0: begin
        SOC <= 1;
        STAR <= (eoc == 0) ? S1 : S0;
      end 
      S1: begin
        SOC <= 0; 
        C <= 0;
        X <= x;
        STAR <= (eoc == 1) ? S2 : S1;
      end
      S2: begin
        C <= C + X[0];
        X <= {2'b00, X[7:2]};
        STAR <= (X == 0) ? S3 : S2;
      end
      S3: begin
        DAV_ <= 0;
        STAR <= ({rfd1, rfd2, rfd3} == 3'b000) ? S4 : S3;
      end
      S4: begin
        DAV_ = 1;
        STAR <= ({rfd2, rfd2, rfd3} == 3'b111) ? S0: S4;
      end
    endcase
  end


endmodule