library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use STD.TEXTIO.all;
-- use work.aux_functions.all;

entity memory_timer is
    port (
        clk         : in  std_logic;
        rst         : in  std_logic;
        address     : in  std_logic_vector(31 downto 0);
        -- address     : in wire32;
        hold_timer  : out std_logic
    );
end memory_timer;

architecture behavioral of memory_timer is
    signal counter : unsigned(4 downto 0);
    type state_type is (WAITING, COUNTING);
    signal current_state : state_type;
    signal aux : std_logic_vector(31 downto 0);
begin

    process(clk, rst)
    begin
        if rst = '1' then
            counter <= (others => '0');
            hold_timer <= '0';
            current_state <= WAITING;
            aux <= (others => '0');
        elsif rising_edge(clk) then
            case current_state is
                when WAITING =>
                    hold_timer <= '0';
                    if address /= aux then
                        current_state <= COUNTING;
                    end if;
                    
                when COUNTING =>
                    hold_timer <= '1';
                    if counter = 15 then 
                        counter <= (others => '0');
                        current_state <= WAITING;
                    else
                        counter <= counter + 1;
                    end if;
            end case;
        end if;
    end process;

end behavioral;