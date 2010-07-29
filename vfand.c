/*
 * vfand.c
 * Copyright (C) 2010 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <linux/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>


/* Our own ioctl definitions: ease compilation */
#define SONYPI_IOCGFAN  _IOR('v', 10, __u8)
#define SONYPI_IOCSFAN  _IOW('v', 11, __u8)
#define SONYPI_IOCGTEMP _IOR('v', 12, __u8)


#ifndef  SONYPI_PATH
# define SONYPI_PATH "/dev/sonypi"
#endif /* !SONYPI_PATH */

#ifndef  POLL_INTERVAL
# define POLL_INTERVAL 2
#endif /* !POLL_INTERVAL */

#ifndef  TEMP_HIGH
# define TEMP_HIGH 50
#endif /* !TEMP_HIGH */

#ifndef  TEMP_LOW
# define TEMP_LOW 30
#endif /* !TEMP_LOW */

#ifdef FANSPEED_MIN
# warning Manually setting FANSPEED_MIN may harm your computer
#else
# define FANSPEED_MIN 0
#endif /* FANSPEED_MIN */

#ifdef FANSPEED_MAX
# warning Manually setting FANSPEED_MAX may harm your computer
#else
# define FANSPEED_MAX 255
#endif /* FANSPEED_MAX */

/* Ugly... */
#define S__(x) #x
#define S_(y) S__(y)


static int         verbose      = 0;
static int         sonypi_fd    = -1;
static const char *sonypi_path  = SONYPI_PATH;
static unsigned    interval     = POLL_INTERVAL;
static unsigned    temp_high    = TEMP_HIGH;
static unsigned    temp_low     = TEMP_LOW;


#define _vfand_help_message \
	"Usage: %s [options]\n" \
	"Control fan depending on system temperature on Vaio laptops.\n" \
	"\n" \
	"  -d DEVICE   Path to device (default: " SONYPI_PATH ")\n" \
	"  -i SECONDS  Polling interval, in seconds (default: " S_(POLL_INTERVAL) ")\n" \
	"  -H DEGREES  Temperature considered \"high\" (default: " S_(TEMP_HIGH) ")\n" \
	"  -L DEGREES  Temperature considered \"low\" (default: " S_(TEMP_LOW) ")\n" \
	"  -v          Print informational messages.\n" \
	"  -h, -?      Show this help text.\n" \
	"\n"

#define _unsigned_arg(_var) \
	do { \
		_var = (unsigned) strtoul (optarg, &endpos, 0); \
		if (*endpos != '\0') { \
			fprintf (stderr, "%s: invalid number '%s'\n", argv[0], optarg); \
			exit (EXIT_FAILURE); \
		} \
	} while (0)



static void
report_data (unsigned temp_low,
             unsigned temp_high,
             unsigned temp_cur,
             unsigned speed_min,
             unsigned speed_max,
             unsigned speed_cur,
             unsigned speed_new)
{
    (void) temp_low;  /* unused */
    (void) temp_high; /* unused */
    (void) speed_min; /* unused */
    (void) speed_max; /* unused */

    printf ("temp %3u C, speed %3u, new speed %3u\n",
            temp_cur, speed_cur, speed_new);
    fflush (stdout);
}


static int
fanspeed_calculate (unsigned temp_low,
                    unsigned temp_high,
                    unsigned temp_cur,
                    unsigned speed_min,
                    unsigned speed_max,
                    unsigned speed_cur)
{
    double slope;
    unsigned result;

    assert (temp_low < temp_high);
    (void) speed_cur; /* unused */

    if (temp_cur < temp_low) {
        result = speed_max;
    }
    else if (temp_cur > temp_high) {
        result = speed_max;
    }
    else {
        /* Calculate using a linear formula */
        slope = (double) (speed_max - speed_min) / (temp_high - temp_low);
        result = (unsigned) slope * (temp_cur - temp_low) + speed_min;
    }

    if (verbose) {
        report_data (temp_low,
                     temp_high,
                     temp_cur,
                     speed_min,
                     speed_max,
                     speed_cur,
                     result);
    }

    return result;
}



void
sonypi_fanspeed_set (int fd, unsigned value)
{
    __u8 value8;

    if (value >= FANSPEED_MAX)
        value = FANSPEED_MAX;
    if (value <= FANSPEED_MIN)
        value = FANSPEED_MIN;

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



int
main (int argc, char **argv)
{
    char *endpos;
    unsigned remain;
    int c;

    while ((c = getopt (argc, argv, "?hvd:i:H:L:")) != -1) {
        switch (c) {
            case 'v':
                verbose = 1;
                break;
            case 'd':
                sonypi_path = optarg;
                break;
            case 'i':
                _unsigned_arg (interval);
                break;
            case 'H':
                _unsigned_arg (temp_high);
                break;
            case 'L':
                _unsigned_arg (temp_low);
                break;
            case '?':
            case 'h':
                printf (_vfand_help_message, argv[0]);
                exit (EXIT_SUCCESS);
            default:
                fprintf (stderr, "%s: unrecognized option '-%c'\n", argv[0], c);
                fprintf (stderr, _vfand_help_message, argv[0]);
                exit (EXIT_FAILURE);
        }
    }

    if ((sonypi_fd = open (sonypi_path, O_RDWR, 0)) == -1) {
        fprintf (stderr, "%s: cannot open '%s': %s\n", argv[0],
                 sonypi_path, strerror(errno));
        exit (EXIT_FAILURE);
    }

    while (interval > 0) {
        /* Adjust */
        sonypi_fanspeed_set (sonypi_fd,
                             fanspeed_calculate (temp_low,
                                                 temp_high,
                                                 sonypi_temperature_get (sonypi_fd),
                                                 FANSPEED_MIN,
                                                 FANSPEED_MAX,
                                                 sonypi_fanspeed_get (sonypi_fd)));

        /* Wait */
        remain = interval;
        do {
            remain = sleep (remain);
        } while (remain > 0 && errno == EINTR);
    }

    if (sonypi_fd != -1) {
        close (sonypi_fd);
    }

    exit (EXIT_SUCCESS);
}


/* vim: expandtab tabstop=4 shiftwidth=4
 */
