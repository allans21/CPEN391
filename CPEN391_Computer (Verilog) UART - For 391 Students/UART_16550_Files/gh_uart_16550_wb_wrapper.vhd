-----------------------------------------------------------------------------
--	Filename:	gh_uart_16550_wb_wrapper.vhd
--
--	Description:
--		This is (ment to be) a wishbone interface 
--		wrapper for a 16550 compatible UART 
--
--	Copyright (c) 2006 by H LeFevre 
--		A VHDL 16550 UART core
--		an OpenCores.org Project
--		free to use, but see documentation for conditions 
--
--	Revision 	History:
--	Revision 	Date       	Author    	Comment
--	-------- 	---------- 	---------	-----------
--	1.0      	02/25/06  	H LeFevre	Initial revision
--	1.1			11/28/17		P.Davies		Added Tri-State data output
-----------------------------------------------------------------------------
library ieee ;
use ieee.std_logic_1164.all ;

entity gh_uart_16550_wb_wrapper is
	port(	  
	-------- wishbone signals ------------
		wb_clk_i  : in std_logic;
		wb_rst_i  : in std_logic;
		wb_stb_i  : in std_logic;
		wb_cyc_i  : in std_logic;
		wb_we_i   : in std_logic;
		wb_adr_i  : in std_logic_vector(2 downto 0);
		wb_dat_i  : in std_logic_vector(7 downto 0);
		
		wb_ack_o  : out std_logic;
		wb_dat_o  : out std_logic_vector(7 downto 0);
	----------------------------------------------------
	------ other I/O -----------------------------------	
		BR_clk  : in std_logic;
		sRX	    : in std_logic;
		CTSn    : in std_logic := '1';
		DSRn    : in std_logic := '1';
		RIn     : in std_logic := '1';
		DCDn    : in std_logic := '1';
		
		sTX     : out std_logic;
		DTRn    : out std_logic;
		RTSn    : out std_logic;
		OUT1n   : out std_logic;
		OUT2n   : out std_logic;
		TXRDYn  : out std_logic;
		RXRDYn  : out std_logic;
		
		IRQ     : out std_logic;
		B_CLK   : out std_logic		
		);
end entity;

architecture a of gh_uart_16550_wb_wrapper is

COMPONENT gh_edge_det is
	PORT(	
		clk : in STD_LOGIC;
		rst : in STD_LOGIC;
		D   : in STD_LOGIC;
		re  : out STD_LOGIC; -- rising edge (need sync source at D)
		fe  : out STD_LOGIC; -- falling edge (need sync source at D)
		sre : out STD_LOGIC; -- sync'd rising edge
		sfe : out STD_LOGIC  -- sync'd falling edge
		);
END COMPONENT;

COMPONENT gh_register_ce is
	GENERIC (size: INTEGER := 8);
	PORT(	
		clk : IN		STD_LOGIC;
		rst : IN		STD_LOGIC; 
		CE  : IN		STD_LOGIC; -- clock enable
		D   : IN		STD_LOGIC_VECTOR(size-1 DOWNTO 0);
		Q   : OUT		STD_LOGIC_VECTOR(size-1 DOWNTO 0)
		);
END COMPONENT;

COMPONENT gh_uart_16550 is
	port(
		clk     : in std_logic;
		BR_clk  : in std_logic;
		rst     : in std_logic;
		CS      : in std_logic;
		WR      : in std_logic;
		ADD     : in std_logic_vector(2 downto 0);
		D       : in std_logic_vector(7 downto 0);
		
		sRX	    : in std_logic;
		CTSn    : in std_logic := '1';
		DSRn    : in std_logic := '1';
		RIn     : in std_logic := '1';
		DCDn    : in std_logic := '1';
		
		sTX     : out std_logic;
		DTRn    : out std_logic;
		RTSn    : out std_logic;
		OUT1n   : out std_logic;
		OUT2n   : out std_logic;
		TXRDYn  : out std_logic;
		RXRDYn  : out std_logic;
		
		IRQ     : out std_logic;
		B_CLK   : out std_logic;
		RD      : out std_logic_vector(7 downto 0)
		);
END COMPONENT;

	
	signal iRD    : std_logic_vector(7 downto 0);
	signal CS     : std_logic;
	signal iCS    : std_logic;
	
	signal InternalDataOut : Std_Logic_Vector(7 downto 0) ;
	
begin




----------------------------------------------
			  
U1 : gh_uart_16550 
	PORT MAP (
		clk    => wb_clk_i,
		BR_clk => BR_clk,
		rst    => wb_rst_i,
		CS     => iCS,
		WR     => wb_we_i,
		ADD    => wb_adr_i,
		D      => wb_dat_i,
		sRX	   => sRX,
		CTSn   => CTSn,
		DSRn   => DSRn,
		RIn    => RIn,
		DCDn   => DCDn,
		
		sTX    => sTX,
		DTRn   => DTRn,
		RTSn   => RTSn,
		OUT1n  => OUT1n,
		OUT2n  => OUT2n,
		TXRDYn => TXRDYn,
		RXRDYn => RXRDYn,
		
		IRQ    => IRQ,
		B_CLK  => B_CLK,
		RD     => iRD
		);

u2 : gh_register_ce 
	generic map (8)
	port map(
		clk => wb_clk_i,
		rst => wb_rst_i,
		ce => iCS,
		D => iRD,
		Q => InternalDataOut
		);
		
 --------------------------------------------------

U3 : gh_edge_det 
	PORT MAP (
		clk => wb_clk_i,
		rst => wb_rst_i,
		d => CS,
		re => iCS);
		
	CS <= '1' when ((wb_stb_i = '1') and (wb_cyc_i = '1')) else
	      '0';

process (wb_clk_i,wb_rst_i)
begin
	if (wb_rst_i = '1') then
		wb_ack_o <= '0';
	elsif (rising_edge(wb_clk_i)) then
		wb_ack_o <= CS;
	end if;
end process ;

-- process to tri-state outputs when not active
-- if reading and devices selected and strobe active, output data, otherwise tri-state

Uart_DataOut: process(wb_stb_i, wb_cyc_i, wb_we_i)
begin
	 if(wb_stb_i = '1' and wb_cyc_i = '1' and wb_we_i = '0') then
		 wb_dat_o <= InternalDataOut ;
	 else
		 wb_dat_o <= "ZZZZZZZZ";   -- else tri-state
	 end if;
end process;
		  
--------------------------------------------------------------

end a;
