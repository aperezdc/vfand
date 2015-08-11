/*
 * sonypi.c
 * Copyright (C) 2010-2015 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "vfand.h"
#include <assert.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#ifndef SONYPI_PATH
#define SONYPI_PATH "/dev/sonypi"
#endif /* !SONYPI_PATH */

/* Our own ioctl definitions: ease compilation */
#define SONYPI_IOCGFAN  _IOR('v', 10, __u8)
#define SONYPI_IOCSFAN  _IOW('v', 11, __u8)
#define SONYPI_IOCGTEMP _IOR('v', 12, __u8)


static int sonypi_fd = -1;


static void
sonypi_destroy (vfand_access_t *access)
{
    (void) access; /* unused */

    assert (access);
    assert (sonypi_fd >= 0);

    close (sonypi_fd);
}


int
sonypi_get_temperature (vfand_access_t *access)
{
    (void) access; /* unused */

    assert (access);
    assert (sonypi_fd >= 0);

    __u8 value;
    if (ioctl (sonypi_fd, SONYPI_IOCGTEMP, &value) < 0) {
        int err = errno;
        fprintf (stderr, "ioctl: %s\n", strerror (err));
        return -err;
    }
    return (int) value;
}


int
sonypi_get_fan_speed (vfand_access_t *access)
{
    (void) access; /* unused */

    assert (access);
    assert (sonypi_fd >= 0);

    __u8 value;
    if (ioctl (sonypi_fd, SONYPI_IOCGFAN, &value) < 0) {
        int err = errno;
        fprintf (stderr, "ioctl: %s\n", strerror (err));
        return -err;
    }
    return (int) value;
}


void
sonypi_set_fan_speed (vfand_access_t *access, int value)
{
    (void) access; /* unused */

    assert (access);
    assert (sonypi_fd >= 0);

    if (value >= MAX_FAN_SPEED)
        value = MAX_FAN_SPEED;
    if (value <= MIN_FAN_SPEED)
        value = MIN_FAN_SPEED;

    __u8 value8 = (__u8) value;
    if (ioctl (sonypi_fd, SONYPI_IOCSFAN, &value8) < 0) {
        fprintf (stderr, "ioctl: %s\n", strerror (errno));
    }
}


vfand_access_t*
vfand_get_sonypi (void)
{
    static vfand_access_t access = {
        .destroy         = sonypi_destroy,
        .get_temperature = sonypi_get_temperature,
        .get_fan_speed   = sonypi_get_fan_speed,
        .set_fan_speed   = sonypi_set_fan_speed,
    };

    static bool initialized = false;
    if (!initialized) {
        sonypi_fd = open (SONYPI_PATH, O_RDWR, 0);
        initialized = true;
    }

    return (sonypi_fd >= 0) ? &access : NULL;
}
