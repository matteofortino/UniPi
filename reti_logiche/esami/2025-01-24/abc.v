module ABC(
    soc1, soc2,
    eoc1, eoc2,
    x1, x2,
    out,
    clock, reset_
);


    input eoc1, eoc2;
    input[7:0] x1, x2;
    input clock, reset_;
    
    output soc1, soc2;
    reg SOC;
    assign soc1 = SOC;
    assign soc2 = SOC;

    output out;
    reg OUT;
    assign out = OUT;

    reg[7:0] COUNT;

    reg[2:0] STAR;
    localparam S0 = 0,
               S1 = 1,
               S2 = 2,
               S3 = 3,
               S4 = 4;

    always @(reset_ == 0) begin
        OUT <= 0;
        SOC <= 0;
        COUNT <= 0;
        STAR <= S0;
    end

    wire[7:0] m;
    MEDIA me (
        .x1(x1), .x2(x2),
        .media(m)
        );

    always @(posedge clock) if(reset_ == 1) begin
        casex (STAR)
            S0: begin
                OUT <= 0;
                SOC <= 1;
                STAR <= ({eoc1, eoc2} ==  2'b00) ? S1 : S0;
            end
            S1: begin
                SOC <= 0;
                COUNT <= m;
                STAR <= ({eoc1, eoc2} ==  2'b11) ? S2 : S1;
            end
            S2: begin
                STAR <= (COUNT == 0) ? S0 : S3;
            end
            S3: begin
                OUT <= 1;
                COUNT <= COUNT - 1;
                STAR <= (COUNT == 1) ? S0 : S3;
            end
        endcase
    end

endmodule


module MEDIA(
    x1, x2, 
    media
);

input[7:0] x1, x2;
output[7:0] media;

wire[8:0] s;
add #(.N(8)) a(
    .x(x1), .y(x2), .c_in(1'b0), 
    .c_out(s[8]), .s(s[7:0])
);

assign media = s[8:1];


endmodule