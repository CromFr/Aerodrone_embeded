#ifndef	WIRINGPI_H_INCLUDED
#define	WIRINGPI_H_INCLUDED

#include "./wiringPi.h"

//
#define WP_GPIO0	8
#define WP_GPIO1	9

#define WP_GPIO4	7

#define WP_GPIO7	11
#define WP_GPIO8	10
#define WP_GPIO9	13
#define WP_GPIO10	12
#define WP_GPIO11	14

#define WP_GPIO14	15
#define WP_GPIO15	16

#define WP_GPIO17	0
#define WP_GPIO18	1

#define WP_GPIO21	2
#define WP_GPIO22	3
#define WP_GPIO23	4
#define WP_GPIO24	5
#define WP_GPIO25	6


//Conveinance defines
#define WP_SDA		WP_GPIO0
#define WP_SCL		WP_GPIO1
#define WP_CPCLK0	WP_GPIO4
#define WP_TXD		WP_GPIO14
#define WP_RXD		WM_GPIO15
#define WP_PCM_CLK	WP_GPIO18
#define WP_PCM_DOUT	WP_GPIO21
#define WP_MOSI		WP_GPIO10
#define WP_MISO		WP_GPIO9
#define WP_SCKL		WP_GPIO11
#define WP_CE0		WP_GPIO8
#define WP_CE1		WP_GPIO7


//Pin count
#define WP_FIRSTPIN	0
#define WP_LASTPIN	16


#endif	//	WIRINGPI_H_INCLUDED
