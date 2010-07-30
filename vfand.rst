=======
 vfand
=======

-------------------------------
Control the fan in Vaio laptops
-------------------------------

:Author: Adrian Perez <aperez@igalia.com>
:Manual section: 8


SYNOPSIS
========

``vfand [options]``


DESCRIPTION
===========

The ``vfand`` daemon monitors your Vaio laptop's system temperature and
adjusts fan speed depending on it. Currently, the adjustment is linear and
is done in the following fashion:

* If the system temperature is less than the *low* threshold, then the
  fan is turned off.

* If the system temperature is more than the *low* threshold, but less than
  the *high* threshold, then the speed is adjusted linearly. Higher
  temperature values will cause the fan speed to increase and vice versa.

* If the system temperature is more than the *high* threshold, then the fan
  is set to full speed.

When the embedded fan controller does not suffice to keep the machine
cooled, or if your machine is noisy, you can use ``vfand``.


USAGE
=====

The daemon accepts a number of command line options:

-d DEVICE   Path to device (default: /dev/sonypi). In general you should
            never chenge this, unless you have a very strange setup.

-i SECONDS  Polling interval, in seconds (default: 2). This is the amount
            of time to wait before two consecutive fan adjustments. If you
            set this too high, your machine could overheat.

-H DEGREES  Temperature considered *high* (default: 55). When the
            temperature is above this value, the fan will be driven to full
            speed.

-L DEGREES  Temperature considered *low* (default: 35). When the temperature
            falls below this value, the fan will be kept at the minimum
            possible speed.

-v          Print informational messages containing temperature, old speed
            and new ones each time the speed is readjusted.

-h, -?      Show a help text and exit


CAVEATS
=======

There is a couple of caveats to keep in mind while using ``vfand``:

* The daemon will *not detach* itself from the controlling terminal. You may
  want to launch it from `inittab(5)`, or with the aid of a process monitor
  like `supervise(8)` or `runsv(8)`. A less beautiful option is using
  `start-stop-daemon(8)`.

* The daemon will log warnings to standard error, and informative messages to
  standard output. If you want to send messages to log files you will need to
  use a script or a third-party tool like `rotlog(8)`.

Actually, those are design decisions: they keep things simple, and the
aforementioned companion tools are better suited for that tasks.


FILES
=====

This program uses the ``/dev/sonypi`` character device. Any other compatible
device which supports the following `ioctl(2)` will work:

* ``SONYPI_IOCGFAN``
* ``SONYPI_IOCSFAN``
* ``SONYPI_IOCGTEMP``


SEE ALSO
========

`supervise(8)`, `runsv(8)`, `start-stop-daemon(8)`, `inittab(5)`,
`rotlog(8)`

http://cr.yp.to/daemontools.html, http://furi-ku.org/+/cgit/code/rotlog




