use library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity cache is 
port(
    clk, reset      : in std_logic;
    address         : in  std_logic_vector (31 downto 0);
    hit, miss       : out std_logic;
);
end cache;

architecture cache of cache is 

begin 


end architecture cache;