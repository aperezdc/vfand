/*
 * vfand.c
 * Copyright (C) 2010-2015 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "vfand.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>


#ifndef  POLL_INTERVAL
# define POLL_INTERVAL 2
#endif /* !POLL_INTERVAL */

#ifndef  TEMP_HIGH
# define TEMP_HIGH 50
#endif /* !TEMP_HIGH */

#ifndef  TEMP_LOW
# define TEMP_LOW 30
#endif /* !TEMP_LOW */

/* Ugly... */
#define S__(x) #x
#define S_(y) S__(y)


static int         verbose      = 0;
static unsigned    interval     = POLL_INTERVAL;
static unsigned    temp_high    = TEMP_HIGH;
static unsigned    temp_low     = TEMP_LOW;


#define _vfand_help_message \
	"Usage: %s [options]\n" \
	"Control fan depending on system temperature on Vaio laptops.\n" \
	"\n" \
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


int
main (int argc, char **argv)
{
    char *endpos;
    unsigned remain;
    int c;

    while ((c = getopt (argc, argv, "?hvi:H:L:")) != -1) {
        switch (c) {
            case 'v':
                verbose = 1;
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

    vfand_access_t *vfand = vfand_get_access (argv[0]);
    if (!vfand) exit (EXIT_FAILURE);

    while (interval > 0) {
        int temperature = vfand_get_temperature (vfand);
        int fan_speed   = vfand_get_fan_speed   (vfand);

        if (temperature >= 0 && fan_speed >= 0) {
            /* Adjust */
            vfand_set_fan_speed (vfand,
                                 fanspeed_calculate (temp_low,
                                                     temp_high,
                                                     (unsigned) temperature,
                                                     MIN_FAN_SPEED,
                                                     MAX_FAN_SPEED,
                                                     fan_speed));
        }

        /* Wait */
        remain = interval;
        do {
            remain = sleep (remain);
        } while (remain > 0 && errno == EINTR);
    }

    vfand_destroy (vfand);
    exit (EXIT_SUCCESS);
}


/* vim: expandtab tabstop=4 shiftwidth=4
 */
