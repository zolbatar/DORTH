# Template Makefile for beebScreen projects, add your
# Tools
CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
RM = rm
ECHO = echo

# Temporary file to hold our unconverted binary file
TEMP = temp
TARGET = Dorth

#Uncomment this line for the full fat VFP optimised Raspberry Pi Native ARM co-processor build.
CFLAGS := -std=gnu99 -nostartfiles -O3 -mfloat-abi=hard -mfpu=vfp -march=armv6zk -mtune=arm1176jzf-s -fno-delete-null-pointer-checks -fdata-sections -ffunction-sections --specs=nano.specs --specs=nosys.specs \
			-Wuninitialized -I capstone/include -DCAPSTONE_HAS_ARM -DPITUBE -DVFP

#Uncomment this line if you find minimised enums are breaking your code's data formats. (default is to make enums as small as possible to hold all values)
CFLAGS += -fno-short-enums

 #These flags work with both builds
LD_FLAGS := -Wl,--gc-sections -Wl,--no-print-gc-sections -Wl,-T,tube/rpi.X -Wl,-lm -Wl,-no-enum-size-warning

# Add your object files here
OBJ = main.o pitube/fb.o pitube/prompt.o pitube/keyboard.o \
compiler/compiler.o compiler/native.o compiler/stack.o \
lightning/lightning.o lightning/jit_memory.o lightning/jit_note.o lightning/jit_size.o lightning/jit_disasm.o lightning/jit_print.o \
capstone/arch/ARM/ARMDisassembler.o capstone/arch/ARM/ARMInstPrinter.o capstone/arch/ARM/ARMMapping.o capstone/arch/ARM/ARMModule.o \
capstone/cs.o capstone/MCInst.o capstone/MCInstrDesc.o capstone/MCRegisterInfo.o capstone/SStream.o capstone/utils.o \
pitube/fonts/bitmap_fonts.o pitube/fonts/font.o pitube/console/console.o

# Beeb coprocessor 
BEEB_OBJ = tube/armc-start.o tube/armtubeio.o tube/armtubeswis.o beebScreen/beebScreen.o

LIB = -lc -lm

$(TARGET): $(OBJ) $(BEEB_OBJ)
	$(CC) $(CFLAGS) $(LD_FLAGS) $(OBJ) $(BEEB_OBJ) $(LIB) -o $(TEMP)
	$(OBJCOPY) -O binary $(TEMP) $@
	$(RM) $(TEMP)
	xxd -i -n Dorth Dorth > ../PiTubeDirect/src/Dorth.h
#	$(ECHO) $(TARGET) 0000F000 0000F000 > $(TARGET).inf
#	$(ECHO) $(TARGET) WR \(0\) F000 F000 > \#\#\#BBC.inf

cleanall: clean depclean 

clean: 
	$(RM) $(OBJ) $(BEEB_OBJ) $(TARGET) beebScreen/*.bin

remake: clean $(TARGET)

beebScreen/beebScreen.o: beebScreen beebScreen beebScreen
	$(CC) $(CFLAGS) -c beebScreen/beebScreen.c -o beebScreen/beebScreen.o

beebScreen/beebCode.c: beebScreen
	cd beebScreen; ./mkasm.sh

beebScreen/extraCode.c: beebScreen beebScreen
	cd beebScreen; ./mkasm.sh

beebScreen/beebCode.bin: beebScreen

beebScreen/extraCode.bin: beebScreen

main.c: beebScreen beebScreen

depclean:
	$(RM) $(OBJ:.o=.d)

%.d: %.c
	$(CC) -M -MG $(DEPFLAGS) $< | sed -e 's@ /[^ ]*@@g' -e 's@^\(.*\)\.o:@\1.d \1.o:@' > $@

include $(SRC:.c=.d)
	
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@
	

