===================
 vfand-temperature
===================

----------------------------------------------------
Report temperature in Vaio laptops as seen by vfand
----------------------------------------------------

:Author: Adrian Perez <aperez@igalia.com>
:Manual section: 8


SYNOPSIS
========

``vfand-temperature [options]``


DESCRIPTION
===========

The ``vfand-temperature`` tool will print out the current system temperature
as reported by the Sony Programmable I/O Control device (SPIC) present in
Vaio computers. This will report the same temperature used by the `vfand(8)`
daemon to adjust fan speed.


USAGE
=====

The daemon accepts a number of command line options:

-i SECONDS  Reporting interval, in seconds (default: 0). This is the amount
            of time to wait before reporting the temperatyre. When zero, the
            temperature will be reported only once, otherwise the tool will
            enter a loop, reporting the temperature at each interval.

-h, -?      Show a help text and exit


FILES
=====

This program uses the ``/dev/sonypi`` character device.


SEE ALSO
========

`vfand(8)`

