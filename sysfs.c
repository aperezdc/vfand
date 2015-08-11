/*
 * sysfs.c
 * Copyright (C) 2015 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#define _POSIX_C_SOURCE 200809L
#include "vfand.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>


#ifndef SYSFS_FANSPEED_PATH
#define SYSFS_FANSPEED_PATH "/sys/devices/platform/sony-laptop/fanspeed"
#endif /* !SYSFS_FANSPEED_PATH */

#ifndef SYSFS_FANSPEED_MIN
#define SYSFS_FANSPEED_MIN 0
#else
#warning Manually setting SYSFS_FANSPEED_MIN may harm your computer
#endif /* !SYSFS_FANSPEED_MIN */

#ifndef SYSFS_FANSPEED_MAX
#define SYSFS_FANSPEED_MAX 255
#else
#warning Manually setting SYSFS_FANSPEED_MAX may harm your computer
#endif /* !SYSFS_FANSPEED_MAX */

#ifndef SYSFS_THERMAL_PATH
#define SYSFS_THERMAL_PATH "/sys/class/thermal/thermal_zone0/temp"
#endif /* !SYSFS_THERMAL_PATH */


struct sysfs_access {
    vfand_access_t parent;
    int            fanspeed_fd;
    int            temperature_fd;
};


static void
sysfs_destroy (vfand_access_t *arg)
{
    struct sysfs_access *access = (struct sysfs_access*) arg;
    assert (access);
    close (access->temperature_fd);
    close (access->fanspeed_fd);
    free (access);
}


static int
sysfs_get_temperature (vfand_access_t *arg)
{
    struct sysfs_access *access = (struct sysfs_access*) arg;
    assert (access);

    char buf[64];
    if (pread (access->temperature_fd, buf, 64, 0) < 0)
        return -1;

    buf[63] = '\0';
    return (int) (strtol (buf, NULL, 10) / 1000.0 + 0.5);
}


static int
sysfs_get_fan_speed (vfand_access_t *arg)
{
    struct sysfs_access *access = (struct sysfs_access*) arg;
    assert (access);

    char buf[4] = { 0, };
    if (pread (access->fanspeed_fd, buf, 4, 0) < 0)
        return -1;

    buf[3] = '\0'; /* Ensure zero termination */
    return (int) strtol (buf, NULL, 10);
}


static void
sysfs_set_fan_speed (vfand_access_t *arg, int speed)
{
    struct sysfs_access *access = (struct sysfs_access*) arg;
    assert (access);

    char buf[4];
    int buf_len = snprintf (buf, 4, "%d", speed);

    /* TODO: Handle write errors? */
    pwrite (access->fanspeed_fd, buf, buf_len, 0);
}


vfand_access_t*
vfand_get_sysfs (void)
{
    int fd = open (SYSFS_FANSPEED_PATH, O_RDWR, 0);
    if (fd < 0) return NULL;

    int temp_fd = open (SYSFS_THERMAL_PATH, O_RDONLY, 0);
    if (temp_fd < 0) return NULL;

    struct sysfs_access *access = malloc (sizeof (struct sysfs_access));
    memset (access, 0x00, sizeof (struct sysfs_access));

    access->fanspeed_fd = fd;
    access->temperature_fd = temp_fd;
    access->parent = (vfand_access_t) {
        .destroy         = sysfs_destroy,
        .get_temperature = sysfs_get_temperature,
        .get_fan_speed   = sysfs_get_fan_speed,
        .set_fan_speed   = sysfs_set_fan_speed,
    };

    return (vfand_access_t*) access;
}

