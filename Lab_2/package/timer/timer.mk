################################################################################
#
# timer
#
################################################################################

TIMER_VERSION = 1.0
TIMER_SITE = $(TOPDIR)/../package/timer_src
TIMER_SITE_METHOD = local
TIMER_LICENSE = Proprietary
TIMER_DEPENDENCIES = libgpiod

$(eval $(cmake-package))
