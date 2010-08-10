/*
 * vfand-temperature.c
 * Copyright (C) 2010 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "sonypi.h"
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

static int         sonypi_fd    = -1;
static const char *sonypi_path  = SONYPI_PATH;
static unsigned    interval     = REPORT_INTERVAL;


#define _vfand_temperature_help_message \
	"Usage: %s [options]\n" \
	"Report system temperature as used by vfand for fan adjustment.\n" \
	"\n" \
	"  -d DEVICE   Path to device (default: " SONYPI_PATH ")\n" \
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
    unsigned remain;
    int c;

    while ((c = getopt (argc, argv, "?hd:i:")) != -1) {
        switch (c) {
            case 'd':
                sonypi_path = optarg;
                break;
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

    if ((sonypi_fd = sonypi_open_device (sonypi_path)) == -1) {
        fprintf (stderr, "%s: cannot open '%s': %s\n", argv[0],
                 sonypi_path, strerror(errno));
        exit (EXIT_FAILURE);
    }

    do {
        printf ("%u\n", sonypi_temperature_get (sonypi_fd));
        fflush (stdout);

        /* Wait */
        remain = interval;
        do {
            remain = sleep (remain);
        } while (remain > 0 && errno == EINTR);
    } while (interval);

    if (sonypi_fd != -1) {
        sonypi_close (sonypi_fd);
    }

    exit (EXIT_SUCCESS);
}


/* vim: expandtab tabstop=4 shiftwidth=4
 */
