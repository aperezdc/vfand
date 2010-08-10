/*
 * sonypi.h
 * Copyright (C) 2010 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef __sonypi_h__
#define __sonypi_h__

#ifndef SONYPI_PATH
#define SONYPI_PATH "/dev/sonypi"
#endif /* !SONYPI_PATH */

#ifndef SONYPI_FANSPEED_MIN
#define SONYPI_FANSPEED_MIN 0
#else
#warning Manually setting SONYPI_FANSPEED_MIN may harm your computer
#endif /* !SONYPI_FANSPEED_MIN */

#ifndef SONYPI_FANSPEED_MAX
#define SONYPI_FANSPEED_MAX 255
#else
#warning Manually setting SONYPI_FANSPEED_MAX may harm your computer
#endif /* !SONYPI_FANSPEED_MAX */


#define  sonypi_open( ) \
         sonypi_open_device(NULL)

int      sonypi_open_device     (const char*);
void     sonypi_close           (int);

void     sonypi_fanspeed_set    (int, unsigned);
unsigned sonypi_fanspeed_get    (int);

unsigned sonypi_temperature_get (int);

#endif /* !__sonypi_h__ */

/* vim: expandtab tabstop=4 shiftwidth=4
 */
