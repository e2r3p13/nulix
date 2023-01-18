// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* drivers/rtc/rtc.c
 *
 * RTC (Real Time Clock) driver
 *
 * created: 2022/12/15 - glafond- <glafond-@student.42.fr>
 * updated: 2023/01/18 - glafond- <glafond-@student.42.fr>
 */

#include <kernel/rtc.h>
#include <kernel/port.h>
#include <kernel/string.h>

enum {
	cmos_addr = 0x70,
	cmos_data = 0x71,
};

enum {
	cmos_reg_sec = 0x00,
	cmos_reg_min = 0x02,
	cmos_reg_hour = 0x04,
	cmos_reg_day = 0x07,
	cmos_reg_mon = 0x08,
	cmos_reg_year = 0x09,
	cmos_reg_A = 0x0A,
	cmos_reg_B = 0x0B,
};

/*
 * Return the number of day since the 1st of january.
 */
static int rtc_get_yday(struct rtc_time *t) {
	int yday = t->day - 1;
	if (t->mon > 1)
		yday += 31;
	if (t->mon > 2) {
		if ((!(t->year % 4)) && ((!(t->year % 400)) || (t->year % 100)))
			yday += 29;
		else
			yday += 28;
	}
	if (t->mon > 3)
		yday += 31;
	if (t->mon > 4)
		yday += 30;
	if (t->mon > 5)
		yday += 31;
	if (t->mon > 6)
		yday += 30;
	if (t->mon > 7)
		yday += 31;
	if (t->mon > 8)
		yday += 31;
	if (t->mon > 9)
		yday += 30;
	if (t->mon > 10)
		yday += 31;
	if (t->mon > 11)
		yday += 30;
	return yday;
}

/*
 * Return the value of a register
 */
static inline uint8_t rtc_get_reg(uint8_t reg) {
	port_write_u8(cmos_addr, reg);
	return port_read_u8(cmos_data); 
}

/*
 * Convert from BCD notation to binary.
 */
static inline void rtc_convert_BCDtobinary(struct rtc_time *tm) {
	tm->sec = (tm->sec & 0x0F) + ((tm->sec / 16) * 10);
	tm->min = (tm->min & 0x0F) + ((tm->min / 16) * 10);
	tm->hour = ((tm->hour & 0x0F) + (((tm->hour & 0x70) / 16) * 10)) | (tm->hour & 0x80);
	tm->day = (tm->day & 0x0F) + ((tm->day / 16) * 10);
	tm->mon = (tm->mon & 0x0F) + ((tm->mon / 16) * 10);
	tm->year = (tm->year & 0x0F) + ((tm->year / 16) * 10);
}

/*
 * Read time from CMOS register and store it in the rtc_time struct.
 */
void rtc_read_time(struct rtc_time *tm) {
	struct rtc_time ltm;
	uint8_t regB;
	uint16_t curyear = 2022;

	memset(tm, 0, sizeof(struct rtc_time));
	do {
		ltm = *tm;
		// Return the 'Update In Progress' flag
		// If 7th bit of CMOS register A is set an update is in progress
		while (rtc_get_reg(cmos_reg_A) & 0x80);

		tm->sec = rtc_get_reg(cmos_reg_sec);
		tm->min = rtc_get_reg(cmos_reg_min);
		tm->hour = rtc_get_reg(cmos_reg_hour);
		tm->day = rtc_get_reg(cmos_reg_day);
		tm->mon = rtc_get_reg(cmos_reg_mon);
		tm->year = rtc_get_reg(cmos_reg_year);
	} while (rtc_isdiff(tm, &ltm));

	regB = rtc_get_reg(cmos_reg_B);

	// If regB bit 3 is not set need to convert from BCD mode to binary
	if (!(regB & 0x04))
		rtc_convert_BCDtobinary(tm);

	// If regB bit 2 and hour last bit is set need to convert from 12 hour clock
	// to 24 hour clock.
	if (!(regB & 0x02) && (tm->hour & 0x80))
		tm->hour = ((tm->hour & 0x7f) + 12) % 24;

	tm->year += (curyear / 100) * 100;
	if (tm->year < curyear)
		tm->year += 100;

	tm->yday = rtc_get_yday(tm);
}

/*
 * Return 1 is the tow times are differents.
 */
int rtc_isdiff(struct rtc_time *tm1, struct rtc_time *tm2) {
	return (tm1->sec != tm2->sec) || (tm1->min != tm2->min) ||
		(tm1->hour != tm2->hour) || (tm1->day != tm2->day) ||
		(tm1->mon != tm2->mon) || (tm1->year != tm2->year);
}

/*
 * Return the number of second since epoch (1 jan 1970)
 */
uint32_t rtc_get_timestamp() {
	struct rtc_time t;
	rtc_read_time(&t);

	uint32_t epoch = 0;
	int year = t.year - 1900;
	epoch += t.sec;
	epoch += t.min * 60;
	epoch += t.hour * 3600;
	epoch += t.yday * 86400;
	epoch += (year-70) * 31536000;
	epoch += ((year - 69) / 4) * 86400;
	epoch -= ((year - 1) / 100) * 86400;
	epoch += ((year + 299) / 400) * 86400;
	return epoch;
}
