`define INIT    2'b00
`define SUM     2'b01
`define SHIFT   2'b10
`define FIM     2'b11

module multiplier (
input clock, reset, start,
input [31:0]A, B,
output reg end_mul,
output reg [64:0]produto
);

reg [64:0] regP;
reg [32:0] regB;
reg [4:0]  cont;
reg [1:0] EA, PE;
wire [32:0] sum;

// Processo do clock
always@(posedge clock, posedge reset)
begin
    if(reset)
        EA = INIT;
    else
        EA = PE;
end


// Processo de troca de estados;
always@*
begin
    case (EA)
        INIT: begin
            if(start == 1)
                PE <= SUM;
            else
                PE <= INIT;
        end
        
        SUM: begin
            PE <= SHIFT;
        end

        SHIFT:begin
            if(cont == 5'd0)
                PE <= FIM;
            else
                PE <= SUM;
        end
        
        
        FIM:begin
            PE <= INIT;
        end
        
        default: 
            PE <= INIT 
    endcase

end


always@(posedge clock, posedge reset)
begin
    if (reset) begin
        regP <= 65'd0;
        regB <= 33'd0;
        cont <= 5'd0;
        end_mul <= 1'b0;
        produto <= 65'd0;
    end

    else begin
        if (EA == INIT) begin
            regB <= {1'b0, B};
            regP <= {33'd0, A};
        end
        
        else if (EA == SUM) begin
            regP <= { sum,regP[31:0]}
            cont <= cont + 5'd1;
        end

        else if (EA == SHIFT) begin
            regP <= { 1'b0 ,regP[:1]};
        end

        else if (EA == FIM) begin
            end_mul <= 1'b1;
            produto <= regP;
        end
    end


end

assign sum = (regP[0] == 1'b1 )? regP[64:32] + regB : regP[64:32];

endmodule
