/*
 * vfand-temperature.c
 * Copyright (C) 2010-2015 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#define _POSIX_C_SOURCE 2
#include "vfand.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


#ifndef REPORT_INTERVAL
#define REPORT_INTERVAL 0
#endif /* !REPORT_INTERVAL */

/* Ugly... */
#define S__(x) #x
#define S_(y) S__(y)

static unsigned interval = REPORT_INTERVAL;


#define _vfand_temperature_help_message \
	"Usage: %s [options]\n" \
	"Report system temperature as used by vfand for fan adjustment.\n" \
	"\n" \
	"  -i SECONDS  Reporting interval, in seconds (default: " S_(REPORT_INTERVAL) ")\n" \
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


int
main (int argc, char **argv)
{
    char *endpos;
    int c;

    while ((c = getopt (argc, argv, "?h:i:")) != -1) {
        switch (c) {
            case 'i':
                _unsigned_arg (interval);
                break;
            case '?':
            case 'h':
                printf (_vfand_temperature_help_message, argv[0]);
                exit (EXIT_SUCCESS);
            default:
                fprintf (stderr, "%s: unrecognized option '-%c'\n", argv[0], c);
                fprintf (stderr, _vfand_temperature_help_message, argv[0]);
                exit (EXIT_FAILURE);
        }
    }


    vfand_access_t* vfand = vfand_get_sonypi ();
    if (!vfand) {
        fprintf (stderr, "%s: cannot open 'sonypi': %s\n", argv[0],
                 strerror(errno));
        exit (EXIT_FAILURE);
    }

    do {
        int temperature = vfand_get_temperature (vfand);
        if (temperature >= 0) {
            printf ("%i\n", temperature);
            fflush (stdout);
        }

        /* Wait */
        unsigned remain = interval;
        do {
            remain = sleep (remain);
        } while (remain > 0 && errno == EINTR);
    } while (interval);


    vfand_destroy (vfand);
    exit (EXIT_SUCCESS);
}


/* vim: expandtab tabstop=4 shiftwidth=4
 */
