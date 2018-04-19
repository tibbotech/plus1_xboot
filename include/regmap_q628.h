#ifndef __INC_REGMAP_Q628_H
#define __INC_REGMAP_Q628_H

struct moon0_regs {
	unsigned int stamp;            // 0.0
	unsigned int clken[10];        // 0.1
	unsigned int gclken[10];       // 0.11
	unsigned int reset[9];         // 0.21
	unsigned int hw_cfg;           // 0.30
	unsigned int sfg_cfg_func;     // 0.31
};
#define MOON0_REG ((volatile struct moon0_regs *)RF_GRP(0, 0))

struct moon1_regs {
	unsigned int sft_cfg[32];
};
#define MOON1_REG ((volatile struct moon1_regs *)RF_GRP(1, 0))

struct moon2_regs {
	unsigned int usbc_ctl;         // 2.0
	unsigned int pllsp_ctl[7];     // 2.1
	unsigned int misc_ctl[2];      // 2.8
	unsigned int dram_size_ctl;    // 2.10
	unsigned int uphy0_ctl[3];;    // 2.11
	unsigned int uphy0_sts;        // 2.14
	unsigned int uphy012_ctl;      // 2.15
	unsigned int plla_ctl[5];      // 2.16
	unsigned int plldis_ctl[4];    // 2.21
	unsigned int pllw_ctl;         // 2.25
	unsigned int pllsys_cfg;       // 2.26
	unsigned int clk_sel0;         // 2.27
	unsigned int rsvd_28[4];       // 2.28
};
#define MOON2_REG ((volatile struct moon2_regs *)RF_GRP(2, 0))

struct pad_ctl_regs {
        unsigned int pad_ctrl[25];
        unsigned int gpio_first[7];    // 4.25
};
#define PAD_CTL_REG ((volatile struct pad_ctl_regs *)RF_GRP(4, 0))

struct hb_gp_regs {
        unsigned int hb_otp_data0;
        unsigned int hb_otp_data1;
        unsigned int hb_otp_data2;
        unsigned int hb_otp_data3;
        unsigned int hb_otp_data4;
        unsigned int hb_otp_data5;
        unsigned int hb_otp_data6;
        unsigned int hb_otp_data7;
        unsigned int hb_otp_ctl;
        unsigned int hb_otp_data;
        unsigned int g7_reserved[22];
};
#define HB_GP_REG ((volatile struct hb_gp_regs *)RF_GRP(350, 0)) //FIXME: q628

struct uart_regs {
        unsigned int dr;  /* data register */
        unsigned int lsr; /* line status register */
        unsigned int msr; /* modem status register */
        unsigned int lcr; /* line control register */
        unsigned int mcr; /* modem control register */
        unsigned int div_l;
        unsigned int div_h;
        unsigned int isc; /* interrupt status/control */
        unsigned int txr; /* tx residue */
        unsigned int rxr; /* rx residue */
        unsigned int thr; /* rx threshold */
};
#define UART0_REG    ((volatile struct uart_regs *)RF_GRP(18, 0))
#define UART1_REG    ((volatile struct uart_regs *)RF_GRP(19, 0))
#define UART2_REG    ((volatile struct uart_regs *)RF_GRP(16, 0))
#define UART3_REG    ((volatile struct uart_regs *)RF_GRP(17, 0))
#define UART4_REG    ((volatile struct uart_regs *)RF_GRP(271, 0))
#define UART5_REG    ((volatile struct uart_regs *)RF_GRP(272, 0))

struct stc_regs {
	unsigned int stc_15_0;       // 12.0
	unsigned int stc_31_16;      // 12.1
	unsigned int stc_64;         // 12.2
	unsigned int stc_divisor;    // 12.3
	unsigned int rtc_15_0;       // 12.4
	unsigned int rtc_23_16;      // 12.5
	unsigned int rtc_divisor;    // 12.6
	unsigned int stc_config;     // 12.7
	unsigned int timer0_ctrl;    // 12.8
	unsigned int timer0_cnt;     // 12.9
	unsigned int timer1_ctrl;    // 12.10
	unsigned int timer1_cnt;     // 12.11
	unsigned int timerw_ctrl;    // 12.12
	unsigned int timerw_cnt;     // 12.13
	unsigned int stc_47_32;      // 12.14
	unsigned int stc_63_48;      // 12.15
	unsigned int timer2_ctl;     // 12.16
	unsigned int timer2_pres_val;// 12.17
	unsigned int timer2_reload;  // 12.18
	unsigned int timer2_cnt;     // 12.19
	unsigned int timer3_ctl;     // 12.20
	unsigned int timer3_pres_val;// 12.21
	unsigned int timer3_reload;  // 12.22
	unsigned int timer3_cnt;     // 12.23
	unsigned int stcl_0;         // 12.24
	unsigned int stcl_1;         // 12.25
	unsigned int stcl_2;         // 12.26
	unsigned int atc_0;          // 12.27
	unsigned int atc_1;          // 12.28
	unsigned int atc_2;          // 12.29
};
#define STC_REG     ((volatile struct stc_regs *)RF_GRP(12, 0))
#define STC_AV0_REG ((volatile struct stc_regs *)RF_GRP(96, 0))
#define STC_AV1_REG ((volatile struct stc_regs *)RF_GRP(97, 0))
#define STC_AV2_REG ((volatile struct stc_regs *)RF_GRP(99, 0))

struct dpll_regs {
	unsigned int dpll0_ctrl;
	unsigned int dpll0_remainder;
	unsigned int dpll0_denominator;
	unsigned int dpll0_divider;
	unsigned int g20_reserved_0[4];
	unsigned int dpll1_ctrl;
	unsigned int dpll1_remainder;
	unsigned int dpll1_denominator;
	unsigned int dpll1_divider;
	unsigned int g20_reserved_1[4];
	unsigned int dpll2_ctrl;
	unsigned int dpll2_remainder;
	unsigned int dpll2_denominator;
	unsigned int dpll2_divider;
	unsigned int g20_reserved_2[4];
	unsigned int dpll3_ctrl;
	unsigned int dpll3_remainder;
	unsigned int dpll3_denominator;
	unsigned int dpll3_divider;
	unsigned int dpll3_sprd_num;
	unsigned int g20_reserved_3[3];
};
#define DPLL_REG     ((volatile struct dpll_regs *)RF_GRP(20, 0))

struct spi_ctrl_regs {
	unsigned int spi_ctrl;       // 22.0
	unsigned int spi_timing;     // 22.1
	unsigned int spi_page_addr;  // 22.2
	unsigned int spi_data;       // 22.3
	unsigned int spi_status;     // 22.4
	unsigned int spi_auto_cfg;   // 22.5
	unsigned int spi_cfg[3];     // 22.6
	unsigned int spi_data_64;    // 22.9
	unsigned int spi_buf_addr;   // 22.10
	unsigned int spi_statu_2;    // 22.11
	unsigned int spi_err_status; // 22.12
	unsigned int spi_data_addr;  // 22.13
	unsigned int mem_parity_addr;// 22.14
	unsigned int spi_col_addr;   // 22.15
	unsigned int spi_bch;        // 22.16
	unsigned int spi_intr_msk;   // 22.17
	unsigned int spi_intr_sts;   // 22.18
	unsigned int spi_page_size;  // 22.19
	unsigned int g20_reserved_0; // 22.20
};
#define SPI_CTRL_REG ((volatile struct spi_ctrl_regs *)RF_GRP(22, 0))

struct uphy_rn_regs {
       unsigned int cfg[22];
};
#define UPHY0_RN_REG ((volatile struct uphy_rn_regs *)RF_GRP(149, 0))
#define UPHY1_RN_REG ((volatile struct uphy_rn_regs *)RF_GRP(150, 0))

/* usb host */
struct ehci_regs {
	unsigned int ehci_len_rev;
	unsigned int ehci_sparams;
	unsigned int ehci_cparams;
	unsigned int ehci_portroute;
	unsigned int g143_reserved_0[4];
	unsigned int ehci_usbcmd;
	unsigned int ehci_usbsts;
	unsigned int ehci_usbintr;
	unsigned int ehci_frameidx;
	unsigned int ehci_ctrl_ds_segment;
	unsigned int ehci_prd_listbase;
	unsigned int ehci_async_listaddr;
	unsigned int g143_reserved_1[9];
	unsigned int ehci_config;
	unsigned int ehci_portsc;
	/*
	unsigned int g143_reserved_2[1];
	unsigned int ehci_version_ctrl;
	unsigned int ehci_general_ctrl;
	unsigned int ehci_usb_debug;
	unsigned int ehci_sys_debug;
	unsigned int ehci_sleep_cnt;
	*/
};
#define EHCI0_REG ((volatile struct ehci_regs *)AHB_GRP(2, 2, 0)) // 0x9c102100
#define EHCI1_REG ((volatile struct ehci_regs *)AHB_GRP(3, 2, 0)) // 0x9c103100

struct usbh_sys_regs {
	unsigned int uhversion;
	unsigned int reserved[3];
	unsigned int uhpowercs_port;
	unsigned int uhc_fsm_axi;
	unsigned int reserved2[22];
	unsigned int uho_fsm_st1;
	unsigned int uho_fsm_st2;
	unsigned int uhe_fsm_st1;
	unsigned int uhe_fsm_st2;
};
#define USBH0_SYS_REG ((volatile struct usbh_sys_regs *)AHB_GRP(2, 0, 0)) // 0x9c102000
#define USBH1_SYS_REG ((volatile struct usbh_sys_regs *)AHB_GRP(3, 0, 0)) // 0x9c103000

struct card_ctl_regs {
	/*g0.0*/
	unsigned int mediatype:3;
	unsigned int reserved0:1;
	unsigned int dmasrc:3;
	unsigned int reserved1:1;
	unsigned int dmadst:3;
	unsigned int reserved2:21;

	/*g0.1*/
	unsigned int card_ctl_page_cnt:16;
	unsigned int reserved3:16;

	/* g0.2 */
	unsigned int sdram_sector_0_size:16;
	unsigned int reserved4:1;
	/* g0.3 */
	unsigned int dma_base_addr;
	/* g0.4 */
	union {
		struct {
			unsigned int reserved5:1;
			unsigned int hw_dma_en:1;
			unsigned int reserved6:1;
			unsigned int hw_sd_hcsd_en:1;
			unsigned int hw_sd_dma_type:2;
			unsigned int hw_sd_cmd13_en:1;
			unsigned int reserved7:1;
			unsigned int stop_dma_flag:1;
			unsigned int hw_dma_rst:1;
			unsigned int dmaidle:1;
			unsigned int dmastart:1;
			unsigned int hw_block_num:2;
			unsigned int reserved8:2;
			unsigned int hw_cmd13_rca:16;
		};
		unsigned int hw_dma_ctl;
	};
	/* g0.5 */
	union {
		struct {
			unsigned int reg_sd_ctl_free:1;				// 0
			unsigned int reg_sd_free:1;					// 1
			unsigned int reg_ms_ctl_free:1;				// 2
			unsigned int reg_ms_free:1;					// 3
			unsigned int reg_dma_fifo_free:1;			// 4
			unsigned int reg_dma_ctl_free:1;			// 5
			unsigned int reg_hwdma_page_free:1;			// 6
			unsigned int reg_hw_dma_free:1;				// 7
			unsigned int reg_sd_hwdma_free:1;			// 8
			unsigned int reg_ms_hwdma_free:1;			// 9
			unsigned int reg_dma_reg_free:1;			// 10
			unsigned int reg_card_reg_free:1;			// 11
			unsigned int reserved9:20;
		};
		unsigned int card_gclk_disable;
	};

	/* g0.6 ~ g0.19*/
	struct {
		unsigned int dram_sector_addr;
		unsigned int sdram_sector_size:16;
		unsigned int reserved10:16;
	} dma_addr_info[7];

	/* g0.20 */
	union {
		struct {
			unsigned int dram_sector_cnt:3;				// 2:00 ro
			unsigned int hw_block_cnt:2;				// 04:03 ro
			unsigned int reserved11:11;					// 15:05 ro
			unsigned int hw_page_cnt:16;				// 31:16 ro 
		};
		unsigned int sdram_sector_block_cnt;
	};
	/* g0.20 ~ g0.28 */
	unsigned int dma_hw_page_addr[4];
	unsigned int dma_hw_page_num[4];

	/* g0.29 */
	unsigned int hw_wait_num;

	/* g0.30 */
	unsigned int hw_delay_num:16;
	unsigned int reserved12:16;

	/* g0.31 */
	union {
		struct {
			unsigned int incnt:11;
			unsigned int outcnt:11;      
			unsigned int dma_sm:3;
			unsigned int reserved13:7;
		};
		unsigned int dma_debug;
	};

	/* g1.0 */
	union {
		struct {
			unsigned int boot_ack_en:1;
			unsigned int boot_ack_tmr:1;
			unsigned int boot_data_tmr:1;
			unsigned int fast_boot:1;
			unsigned int boot_mode:1;
			unsigned int bootack:3;
			unsigned int reserved14:24;
		};
		unsigned int boot_ctl;	
	};

	/* g1.1 */
	union {
		struct {
			unsigned int vol_tmr:2;
			unsigned int sw_set_vol:1;
			unsigned int hw_set_vol:1;
			unsigned int vol_result:2;
			unsigned int reserved15:26;	   
		};
		unsigned int sd_vol_ctrl;
	};
	/* g1.2 */
	union {
		struct {
			unsigned int sd_cmp:1;   //1
			unsigned int sd_cmp_clr:1;   //2
			unsigned int sdio_int_en:1;  //3
			unsigned int sdio_int:1; //4
			unsigned int sdio_int_clr:1; //5
			unsigned int detect_int_en:1;    //6
			unsigned int detect_int:1;   //7
			unsigned int detect_int_clr:1;   //8
			unsigned int hwdmacmpen:1;   //9
			unsigned int hw_dma_cmp:1;   //10
			unsigned int hwdmacmpclr:1;  //11
			unsigned int reserved16:20; //31:12			
		};
		unsigned int sd_int;
	};

	/* g1.3 */
	unsigned int sd_page_num:16;
	unsigned int reserved17:16;
	/* g1.4 */
	union {
		struct {	
			unsigned int sdpiomode:1;
			unsigned int sdddrmode:1;
			unsigned int sd_len_mode:1;
			unsigned int first_dat_hcyc:1;
			unsigned int sd_trans_mode:2;
			unsigned int sdautorsp:1;
			unsigned int sdcmddummy:1;
			unsigned int sdrspchk_en:1;
			unsigned int sdiomode:1;
			unsigned int sdmmcmode:1;
			unsigned int sddatawd:1;
			unsigned int sdrsptmren:1;
			unsigned int sdcrctmren:1;
			unsigned int rx4_en:1;
			unsigned int sdrsptype:1;
			unsigned int detect_tmr:2;
			unsigned int mmc8_en:1;
			unsigned int selci:1;
			unsigned int sdfqsel:12;
		};
		unsigned int sd_config0;
	};

	/* g1.5 */
	union {
		struct {
			unsigned int rwc:1;
			unsigned int s4mi:1;
			unsigned int resu:1;
			unsigned int sus_req:1;
			unsigned int con_req:1;
			unsigned int sus_data_flag:1;
			unsigned int int_multi_trig:1;
			unsigned int reserved18:25; 
		};
		unsigned int sdio_ctrl;
	};

	/* g1.6 */
	union {
		struct {
			unsigned int sdrst:1;
			unsigned int sdcrcrst:1;
			unsigned int sdiorst:1;
			unsigned int reserved19:29; 
		};
		unsigned int sd_rst;
	};

	/* g1.7 */
	union {
		struct {
			unsigned int sdctrl0:1;
			unsigned int sdctrl1:1;
			unsigned int sdioctrl:1;
			unsigned int emmcctrl:1;
			unsigned int reserved20:28;	 
		} ;
		unsigned int sd_ctrl;
	};
	/* g1.8 */
	union {
		struct {
			unsigned int sdstatus:19;
			unsigned int reserved21:13;	 
		};
		unsigned int sd_status;
	};
	/* g1.9 */
	union {
		struct {
			unsigned int sdstate:3;
			unsigned int reserved22:1; 
			unsigned int sdcrdcrc:3; 
			unsigned int reserved23:1; 
			unsigned int sdstate_new:7; 
			unsigned int reserved24:17; 
		};
		unsigned int sd_state;
	};

	/* g1.10 */
	union {
		struct {
			unsigned int hwsd_sm:10;	
			unsigned int reserved25:22; 
		}; 
		unsigned int sd_hw_state;
	};

	/* g1.11 */
	union {
		struct {
			unsigned int sddatalen:11;	
			unsigned int reserved26:21; 
		}; 
		unsigned int sd_blocksize;
	};
	
	/* g1.12 */
	union {
		struct {
			unsigned int tx_dummy_num:9;
			unsigned int sdcrctmr:11;
			unsigned int sdrsptmr:11;
			unsigned int sd_high_speed_en:1;
		}; 
		unsigned int sd_config1;
	};

	/* g1.13 */
	union {
		struct {
			unsigned int sd_clk_dly_sel:3;
			unsigned int reserved27:1;
			unsigned int sd_wr_dly_sel:3;
			unsigned int reserved28:1;    
			unsigned int sd_rd_dly_sel:3;
			unsigned int reserved29:21;
		}; 
		unsigned int sd_timing_config;
	};

	/* g1.14 */
	unsigned int sd_rxdattmr:29; 
	unsigned int reserved30:3;
	
	/* g1.15 */
	unsigned int sd_piodatatx:16;
	unsigned int reserved31:16;

	/* g1.16 */
	unsigned int sd_piodatarx;

	/* g1.17 */
	/* g1.18 */
	unsigned char sd_cmdbuf[5];
	unsigned char reserved32[3];
	/* g1.19 - g1.20 */	
	unsigned int sd_rspbuf0_3;
	unsigned int sd_rspbuf4_5;
	/*  unused sd control regs */
	unsigned int reserved34[11];
	/* ms card related regs */	
	unsigned int ms_regs[32];
};
#define CARD0_CTL_REG ((volatile struct card_control_reg *)RF_GRP(118, 0))
#define CARD1_CTL_REG ((volatile struct card_control_reg *)RF_GRP(125, 0))


/* NAND */
#define NAND_S330_BASE           0x9C100000
#define BCH_S338_BASE_ADDRESS    0x9C101000

/* SPI NAND */
#define CONFIG_SP_SPINAND_BASE   ((volatile unsigned int *)RF_GRP(87, 0))

/* BIO */
struct bio_ctl_regs {
	unsigned int ch0_addr; // 0x9c10_5000
	unsigned int ch0_ctrl; // 0x9c10_5004
	unsigned int ch1_addr; // 0x9c10_5008
	unsigned int ch1_ctrl; // 0x9c10_500c
	unsigned int ch2_addr; // 0x9c10_5010
	unsigned int ch2_ctrl; // 0x9c10_5014
	unsigned int ch3_addr; // 0x9c10_5018
	unsigned int ch3_ctrl; // 0x9c10_501c
	unsigned int ch4_addr; // 0x9c10_5020
	unsigned int ch4_ctrl; // 0x9c10_5024
	unsigned int ch5_addr; // 0x9c10_5028
	unsigned int ch5_ctrl; // 0x9c10_502c
	unsigned int io_ctrl ; // 0x9c10_5030
	unsigned int io_tctl ; // 0x9c10_5034
	unsigned int macro_c0; // 0x9c10_5038
	unsigned int macro_c1; // 0x9c10_503c
	unsigned int macro_c2; // 0x9c10_5040
	unsigned int io_tpctl; // 0x9c10_5044
	unsigned int io_rpctl; // 0x9c10_5048
	unsigned int boot_ra ; // 0x9c10_504c
	unsigned int io_tdat0; // 0x9c10_5050
	unsigned int reserved[11];
};
#define BIO_CTL_REG ((volatile struct bio_ctl_regs *)0x9c105000)

struct A_moon0_regs {
	unsigned int stamp;            // 0.0
	unsigned int ca7_cfg;          // 0.1
	unsigned int ca7_sw_rst;       // 0.2
	unsigned int clk_cfg;          // 0.3
	unsigned int aud_dsp_clk_cfg;  // 0.4
	unsigned int pfcnt_sensor_reg[2]; // 0.5
	unsigned int ca7_standbywfi;   // 0.7
	unsigned int rst_b_0;          // 0.8
	unsigned int clk_en0;          // 0.9
	unsigned int gclk_en0;         // 0.10
	unsigned int pll_ctl[2];       // 0.11
	unsigned int pad_reg;          // 0.13
	unsigned int pad_ds_cfg0;      // 0.14
	unsigned int sys_sleep_div;    // 0.15
	unsigned int pfcnt_ctl;        // 0.16
	unsigned int pfcnt_sts;        // 0.17
	unsigned int ioctrl_cfg;       // 0.18
	unsigned int memctl_cfg;       // 0.19
	unsigned int ca7_ctl_cfg;      // 0.20
	unsigned int pllio_ctl[2];     // 0.21
	unsigned int aud_dsp_otp_reg[2]; // 0.23
	unsigned int rsv[7];           // 0.25
};
#define A_MOON0_REG ((volatile struct A_moon0_regs *)A_RF_GRP(0, 0))

#endif /* __INC_REGMAP_Q628_H */
