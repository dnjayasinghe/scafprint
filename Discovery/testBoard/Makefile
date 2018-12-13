# Sources

SRCS = main.c mini-gmp.c

# USB
SRCS += stm32f4xx_it.c system_stm32f4xx.c syscalls.c usbd_usr.c usbd_cdc_vcp.c usbd_desc.c usb_bsp.c  

# Project name

PROJ_NAME=stm32f4_usb_cdc_mini_ecsm
OUTPATH=build

###################################################

# Check for valid float argument
# NOTE that you have to run make clean after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

###################################################

BINPATH=/home/darshana/Documents/tools/stlink
CC= arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size
STLINK_HOME=/home/darshana/Documents/tools/stlink


# Choose debug or release...
CFLAGS = -g -O0 -fomit-frame-pointer           # Normal
#CFLAGS = -ggdb -O1 -fomit-frame-pointer    # RSW - for GDB debugging, disable optimizer


CFLAGS += -std=gnu99 -Wall -Tstm32_flash.ld
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -nostartfiles -mcpu=cortex-m4 -I/home/zak/articles_2014/2015/15_XXX_FIAGCM/ECDSA/mini-ecsm/STM32F4_USB_CDC/src/

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant 

CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
else
CFLAGS += -msoft-float
endif

###################################################

vpath %.c src
vpath %.a lib

ROOT=$(shell pwd)

# Includes
CFLAGS += -Iinc -Ilib/Core/cmsis -Ilib/Core/stm32
CFLAGS += -Ilib/Conf 

# Library paths
LIBPATHS = -Llib/StdPeriph -Llib/USB_Device/Core
LIBPATHS += -Llib/USB_Device/Class/cdc -Llib/USB_OTG -Llib/aes -Llib/aes0 

# Libraries to link
LIBS = -lm -lstdperiph -lusbdevcore -lusbdevcdc -lusbcore -laes -laes0

# Extra includes
CFLAGS += -Ilib/StdPeriph/inc
CFLAGS += -Ilib/USB_OTG/inc
CFLAGS += -Ilib/USB_Device/Core/inc
CFLAGS += -Ilib/USB_Device/Class/cdc/inc

# add startup file to build
SRCS += lib/startup_stm32f4xx.s

OBJS = $(SRCS:.c=.o)

###################################################

.PHONY: lib proj all clean write test

all: lib proj
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

lib:
	$(MAKE) -C lib FLOAT_TYPE=$(FLOAT_TYPE)

proj: 	$(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBPATHS) $(LIBS)
	$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin

clean:
	rm -f *.o
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin
	$(MAKE) clean -C lib # Remove this line if you don't want to clean the libs as well

write: all
	$(STLINK_HOME)/st-flash write  $(OUTPATH)/$(PROJ_NAME).bin 0x08000000

test: all
	python target_stm32.py
