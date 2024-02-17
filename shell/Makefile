CROSS=pdp11-aout-

ROOT = ..

BINARIES += main.bin

OPT     += -m45
OPT     += -Os
COPT    += -std=gnu17
CPPOPT  += -std=gnu++17
OPT     += -Wall

INC += -I ../include

OBJS += crt0.o
OBJS += main.o
OBJS += bear.o
OBJS += printf.o
OBJS += getline.o
OBJS += uart.o
OBJS += interp.o
OBJS += dump.o
OBJS += ../MyLib/strcmp.o
OBJS += ../Sprintf/sprintf.o

LDSCRIPT=pdp11.ld

all: $(BINARIES)

%.o: %.cpp
	@echo [C+]  $<
	@$(CROSS)g++ $(OPT) $(CPPOPT) $(OPT.$<) $(INC) $(DEFINES) $(DEFINES.$<) $(WARN) -c $<

%.o: %.c
	@echo [CC]  $<
	@$(CROSS)gcc $(OPT) $(COPT) $(OPT.$<) $(INC) $(DEFINES) $(DEFINES.$<) $(WARN) -c -o $@ $<

%.o: %.S
	@echo [AS]  $<
	@$(CROSS)gcc $(OPT) $(OPT.$<) $(INC) $(DEFINES) $(DEFINES.$<) $(WARN) -c $<

main.aout : $(OBJS)
	@echo [LD] $@
	@$(CROSS)gcc -o $@ -T $(LDSCRIPT) -nostartfiles $^ -L ../MyLib -lMyLib -lgcc -Wl,-Map=$(basename $@).map
	@$(CROSS)objdump -d $@ >$(basename $@).list

%.bin : %.aout
	@echo [OBJCOPY] $<
	@$(CROSS)objcopy -O binary --only-section=.text --only-section=.data $< $@


clean:
	@echo [CLEAN]
	@rm -f *.o *.aout *.bin *.map *.list $(OBJS)
