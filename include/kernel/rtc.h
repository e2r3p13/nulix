// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/rtc.h
 *
 * RTC (Real Time Clock) driver header
 *
 * created: 2022/12/15 - glafond- <glafond-@student.42.fr>
 * updated: 2022/12/15 - glafond- <glafond-@student.42.fr>
 */

#ifndef RTC_H
#define RTC_H

struct rtc_time {
	int sec;
	int min;
	int hour;
	int day;
	int mon;
	int year;
};

/*
 * Get RTC tiime and fill rtc_time struct
 */
void rtc_read_time(struct rtc_time *tm);

/*
 * Return diff between to rtc_time
 */
int rtc_isdiff(struct rtc_time *tm1, struct rtc_time *tm2);

#endif
