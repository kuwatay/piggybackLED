###############################################################################
# Makefile for the project piggybackLED
###############################################################################

## General Flags
PROJECT = 5x7matrix
MCU = attiny4313
CLK = 8000000UL

TARGET = $(PROJECT).elf
CC = avr-gcc

AVRDUDE = avrdude -c usbtiny -p$(MCU)

FUSEH = 0xdf
FUSEL = 0x64
# Fuse high byte:
# 0xcf = 1 1 0 1   1 1 1 1 <-- RSTDISB (External Reset disable)
#        ^ ^ ^ ^   ^ ^ ^------ BODLEVEL0
#        | | | |   | +-------- BODLEVEL1
#        | | | |   + --------- BODLEVEL2
#        | | | +-------------- WDTON (WDT not always on)
#        | | +---------------- SPIEN (allow serial programming)
#        | +------------------ EESAVE (preserve EEPROM over chip erase)
#        +-------------------- DWEN (debugWIRE Enable)
# Fuse low byte:
# 0xef = 0 1 1 0   0 1 0 0
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (internal RC isc 8M)
#        | |  +--------------- SUT 1..0 (crystal osc, BOD enabled)
#        | +------------------ CKOUT (1 = diable)
#        +-------------------- CKDEV8 (CLOCK DEV by 8)

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU) -DF_CPU=$(CLK)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -O0
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0


## Objects that must be built in order to link
OBJECTS = matrix.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) $(PROJECT).hex $(PROJECT).eep size

## Compile
matrix.o.: matrix.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: %.elf
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: %.elf
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@

%.lss: %.elf
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(PROJECT).elf dep $(PROJECT).hex $(PROJECT).eep

## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

flash: all
	$(AVRDUDE) -Uflash:w:$(PROJECT).hex

.PHONY: fuse
fuse:
	$(AVRDUDE) -U hfuse:w:$(FUSEH):m -U lfuse:w:$(FUSEL):m
