cat << 'EOF' > simprims.vhd
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Declaração do pacote de componentes
package VCOMPONENTS is
  component X_BUF is
    generic (PATHPULSE : time := 0 ps);
    port (I : in STD_LOGIC; O : out STD_LOGIC);
  end component;

  component X_OBUF is
    port (I : in STD_LOGIC; O : out STD_LOGIC);
  end component;

  component X_MUX2 is
    port (IA, IB, SEL : in STD_LOGIC; O : out STD_LOGIC);
  end component;

  component X_ONE is
    port (O : out STD_LOGIC);
  end component;

  component X_ROC is
    generic (ROC_WIDTH : time := 100 ns);
    port (O : out STD_LOGIC);
  end component;

  component X_TOC is
    port (O : out STD_LOGIC);
  end component;

  component X_LUT4 is
    generic (INIT : bit_vector(15 downto 0) := X"0000");
    port (ADR0, ADR1, ADR2, ADR3 : in STD_LOGIC; O : out STD_LOGIC);
  end component;
end package VCOMPONENTS;

package VPACKAGE is
end package VPACKAGE;

-- Implementação das entidades
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity X_BUF is
  generic (PATHPULSE : time := 0 ps);
  port (I : in STD_LOGIC; O : out STD_LOGIC);
end entity;
architecture RTL of X_BUF is
begin
  O <= I;
end architecture;

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
entity X_OBUF is
  port (I : in STD_LOGIC; O : out STD_LOGIC);
end entity;
architecture RTL of X_OBUF is
begin
  O <= I;
end architecture;

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
entity X_MUX2 is
  port (IA, IB, SEL : in STD_LOGIC; O : out STD_LOGIC);
end entity;
architecture RTL of X_MUX2 is
begin
  O <= IA when SEL = '0' else IB;
end architecture;

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
entity X_ONE is
  port (O : out STD_LOGIC);
end entity;
architecture RTL of X_ONE is
begin
  O <= '1';
end architecture;

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
entity X_ROC is
  generic (ROC_WIDTH : time := 100 ns);
  port (O : out STD_LOGIC);
end entity;
architecture RTL of X_ROC is
begin
  O <= '1', '0' after ROC_WIDTH;
end architecture;

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
entity X_TOC is
  port (O : out STD_LOGIC);
end entity;
architecture RTL of X_TOC is
begin
  O <= '1', '0' after 0 ns;
end architecture;

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity X_LUT4 is
  generic (INIT : bit_vector(15 downto 0) := X"0000");
  port (ADR0, ADR1, ADR2, ADR3 : in STD_LOGIC; O : out STD_LOGIC);
end entity;
architecture RTL of X_LUT4 is
begin
  process(ADR0, ADR1, ADR2, ADR3)
    variable idx : integer range 0 to 15;
    variable addr : std_logic_vector(3 downto 0);
  begin
    addr := ADR3 & ADR2 & ADR1 & ADR0;
    if is_x(addr) then
      O <= 'X';
    else
      idx := to_integer(unsigned(addr));
      if INIT(idx) = '1' then
        O <= '1';
      else
        O <= '0';
      end if;
    end if;
  end process;
end architecture;
EOF