BINPATH=~/sat/bin

CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar

###################################################

# Check for valid float argument
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

###################################################

vpath %.c src

CFLAGS  = -g -O2 -Wall
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -mcpu=cortex-m4

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
else
CFLAGS += -msoft-float
endif

CFLAGS += -ffreestanding -nostdlib

# Includes
CFLAGS += -Iinc -I./
#CFLAGS += -I../../Conf -I../../USB_OTG/inc

# Sources
SRCS = aes.c

OBJS = $(SRCS:.c=.o)
LIBNAME = libaes0.a

all: $(LIBNAME)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(LIBNAME): $(OBJS)
	$(AR) -r $@ $(OBJS)

clean:
	rm -f $(OBJS) $(LIBNAME)

# DO NOT DELETE THIS LINE -- make depend depends on it.


#aes_core.o: ../../inc/aes.h ../../inc/e_os2.h
#aes_core.o: ../../inc/opensslconf.h aes_core.c aes_locl.h

