/*
 * vfand.h
 * Copyright (C) 2015 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef VFAND_H
#define VFAND_H

typedef struct vfand_access vfand_access_t;

struct vfand_access {
    void (*destroy)         (vfand_access_t*);
    int  (*get_temperature) (vfand_access_t*);
    int  (*get_fan_speed)   (vfand_access_t*);
    void (*set_fan_speed)   (vfand_access_t*, int);
};

enum {
    MAX_FAN_SPEED = 255,
    MIN_FAN_SPEED = 0,
};


static inline void
vfand_destroy (vfand_access_t *access) {
    (*access->destroy) (access);
}

static inline int
vfand_get_temperature (vfand_access_t *access) {
    return (*access->get_temperature) (access);
}

static inline int
vfand_get_fan_speed (vfand_access_t *access) {
    return (*access->get_fan_speed) (access);
}

static inline void
vfand_set_fan_speed (vfand_access_t *access, int speed) {
    (*access->set_fan_speed) (access, speed);
}


extern vfand_access_t* vfand_get_sonypi (void);
extern vfand_access_t* vfand_get_sysfs (void);


#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <stdio.h>
#include <errno.h>


static inline vfand_access_t*
vfand_get_access (const char *errprefix)
{
    vfand_access_t *vfand = vfand_get_sonypi ();
    if (vfand) {
        fprintf (stderr, "%s: using 'sonypi'\n", errprefix);
        return vfand;
    }

    fprintf (stderr, "%s: cannot open 'sonypi': %s\n",
             errprefix, strerror(errno));

    if ((vfand = vfand_get_sysfs ())) {
        fprintf (stderr, "%s: using 'sysfs'\n", errprefix);
        return vfand;
    }

    fprintf (stderr, "%s: cannot open 'sysfs': %s\n",
             errprefix, strerror (errno));
    return NULL;
}

#endif /* !VFAND_H */
