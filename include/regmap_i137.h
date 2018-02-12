#ifndef __INC_REGMAP_I137_H
#define __INC_REGMAP_I137_H

struct moon0_regs {
	unsigned int stamp;            // 0.0
	unsigned int g0_reserved_0[3]; // 0.1
	unsigned int clk_sel[2];       // 0.4
	unsigned int pllsys_cfg_sel;   // 0.6
	unsigned int clken[5];         // 0.7
	unsigned int gclken[5];        // 0.12
	unsigned int reset[5];         // 0.17
	unsigned int g0_reserved_2[6]; // 0.22
	unsigned int hw_cfg;           // 0.28
	unsigned int sfg_cfg_func;     // 0.29
	unsigned int g0_reserved_3[2]; // 0.30
};
#define MOON0_REG ((volatile struct moon0_regs *)RF_GRP(0, 0))

struct moon1_regs {
	unsigned int sft_cfg[32];
};
#define MOON1_REG ((volatile struct moon1_regs *)RF_GRP(1, 0))

struct moon2_regs {
	unsigned int sft_cfg[32];
};
#define MOON2_REG ((volatile struct moon2_regs *)RF_GRP(2, 0))

struct pad_ctl_regs {
	unsigned int pad_ctrl[25];
	unsigned int gpio_first[7];
};
#define PAD_CTL_REG ((volatile struct pad_ctl_regs *)RF_GRP(4, 0))

#if 0
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
#define HB_GP_REG ((volatile struct hb_gp_regs *)RF_GRP(350, 0))
#endif

struct iop_regs {
	// Group 8 : IOP
	unsigned int iop_control;
	unsigned int g8_reserved_0;
	unsigned int iop_bp;
	unsigned int iop_regsel;
	unsigned int iop_regout;
	unsigned int iop_memlimit;
	unsigned int g8_reserved_1[2];
	unsigned int iop_data[12];
	unsigned int iop_base_adr_l;
	unsigned int iop_base_adr_h;
	unsigned int memory_bridge_control;
	unsigned int iop_remap_adr_l;
	unsigned int iop_remap_adr_h;
	unsigned int g8_reserved_2[7];
};
#define IOP_REG ((volatile struct iop_regs *)RF_GRP(8, 0))

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
	unsigned int spi_wait;       // 22.1
	unsigned int spi_cust_cmd;   // 22.2
	unsigned int spi_addr_l;     // 22.3
	unsigned int spi_addr_h;     // 22.4
	unsigned int spi_data_l;     // 22.5
	unsigned int spi_data_h;     // 22.6
	unsigned int spi_status;     // 22.7
	unsigned int spi_cfg[9];     // 22.8
	unsigned int spi_cust_cmd_2; // 22.17
	unsigned int spi_data_64;    // 22.18
	unsigned int spi_buf_addr;   // 22.19
	unsigned int spi_status_2;   // 22.20
	unsigned int spi_status_3;   // 22.21
	unsigned int spi_mode_status;// 22.22
	unsigned int spi_err_status; // 22.23
};
#define SPI_CTRL_REG ((volatile struct spi_ctrl_regs *)RF_GRP(22, 0))

struct spind_ctrl_regs {
	unsigned int spi_ctrl;       // 87.0
	unsigned int spi_wait;       // 87.1
	unsigned int spi_cust_cmd;   // 87.2
	unsigned int spi_addr_l;     // 87.3
	unsigned int spi_addr_h;     // 87.4
	unsigned int spi_data_l;     // 87.5
	unsigned int spi_data_h;     // 87.6
	unsigned int spi_status;     // 87.7
	unsigned int spi_cfg[9];     // 87.8
	unsigned int spi_cust_cmd_2; // 87.17
	unsigned int spi_data_64;    // 87.18
	unsigned int spi_rbuf_addr;  // 87.19
	unsigned int spi_status_2;   // 87.20
	unsigned int spi_status_3;   // 87.21
	unsigned int spi_mode_status;// 87.22
	unsigned int spi_err_status; // 87.23
	unsigned int spi_threshold;  // 87.24
	unsigned int spi_finish;     // 87.25
	unsigned int spi_int;        // 87.26
	unsigned int spi_wbuf_addr;  // 87.27
};
#define SPIND_REG ((volatile struct spind_ctrl_regs *)RF_GRP(87, 0))

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

#endif /* __INC_REGMAP_GEMINI_H */
