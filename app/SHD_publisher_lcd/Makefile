# Publisher SHD
APPLICATION = Task2

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_ipv6_default
# Include MQTT-SN
USEMODULE += emcute
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# For testing we also include the ping6 command and some stats
USEMODULE += gnrc_icmpv6_echo
CFLAGS += -DGNRC_IPV6_NIB_CONF_SLAAC=1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer
FEATURES_REQUIRED += periph_adc

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo-f031k6 telosb \
                             wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += xtimer

USEPKG += u8g2

# set default device parameters in case they are undefined
TEST_OUTPUT ?= 3

TEST_I2C ?= 0
TEST_SPI ?= 1

TEST_ADDR ?= 0x3c

TEST_PIN_CS ?= GPIO_PIN\(0,15\)
TEST_PIN_DC ?= GPIO_PIN\(0,8\)
TEST_PIN_RESET ?= GPIO_PIN\(0,16\)

ifeq ($(TEST_OUTPUT),3)
TEST_DISPLAY ?= u8g2_Setup_pcd8544_84x48_1
endif

ifeq ($(TEST_OUTPUT),4)
TEST_DISPLAY ?= u8g2_Setup_pcd8544_84x48_1
endif

# features depend on output type
ifeq ($(TEST_OUTPUT),1)
USEMODULE += u8g2_utf8
endif

ifeq ($(TEST_OUTPUT),2)
USEMODULE += u8g2_sdl
endif

ifeq ($(TEST_OUTPUT),3)
FEATURES_REQUIRED += periph_gpio periph_spi
endif

ifeq ($(TEST_OUTPUT),4)
FEATURES_REQUIRED += periph_gpio periph_i2c
endif

# export parameters
CFLAGS += -DTEST_OUTPUT=$(TEST_OUTPUT)

CFLAGS += -DTEST_SPI=$(TEST_SPI)
CFLAGS += -DTEST_I2C=$(TEST_I2C)

CFLAGS += -DTEST_ADDR=$(TEST_ADDR)

CFLAGS += -DTEST_PIN_CS=$(TEST_PIN_CS)
CFLAGS += -DTEST_PIN_DC=$(TEST_PIN_DC)
CFLAGS += -DTEST_PIN_RESET=$(TEST_PIN_RESET)

CFLAGS += -DTEST_DISPLAY=$(TEST_DISPLAY)

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py



