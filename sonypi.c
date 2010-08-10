/*
 * sonypi.c
 * Copyright (C) 2010 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "sonypi.h"
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>


/* Our own ioctl definitions: ease compilation */
#define SONYPI_IOCGFAN  _IOR('v', 10, __u8)
#define SONYPI_IOCSFAN  _IOW('v', 11, __u8)
#define SONYPI_IOCGTEMP _IOR('v', 12, __u8)


int
sonypi_open_device (const char *path)
{
    if (path == NULL)
        path = SONYPI_PATH;

    return open (path, O_RDWR, 0);
}


void
sonypi_close (int fd)
{
    assert (fd > -1);
    close (fd);
}


void
sonypi_fanspeed_set (int fd, unsigned value)
{
    __u8 value8;

    if (value >= SONYPI_FANSPEED_MAX)
        value = SONYPI_FANSPEED_MAX;
    if (value <= SONYPI_FANSPEED_MIN)
        value = SONYPI_FANSPEED_MIN;

    value8 = (__u8) value;

    if (ioctl (fd, SONYPI_IOCSFAN, &value8) < 0) {
        fprintf (stderr, "ioctl: %s\n", strerror (errno));
    }
}


unsigned
sonypi_fanspeed_get (int fd)
{
    __u8 value;
    if (ioctl (fd, SONYPI_IOCGFAN, &value) < 0) {
        fprintf (stderr, "ioctl: %s\n", strerror (errno));
    }
    return (unsigned) value;
}


unsigned
sonypi_temperature_get (int fd)
{
    __u8 value;
    if (ioctl (fd, SONYPI_IOCGTEMP, &value) < 0) {
        fprintf (stderr, "ioctl: %s\n", strerror (errno));
    }
    return (unsigned) value;
}


/* vim: expandtab tabstop=4 shiftwidth=4
 */
