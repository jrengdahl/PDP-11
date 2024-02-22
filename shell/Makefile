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
OBJS += interp.o
OBJS += threads11.o
OBJS += threadtest.o
OBJS += bear.o
OBJS += printf.o
OBJS += getline.o
OBJS += uart.o
OBJS += dump.o
OBJS += ../MyLib/strcmp.o
OBJS += ../Sprintf/sprintf.o

LDSCRIPT=pdp11.ld

all: $(BINARIES)

-include $(OBJS:.o=.d)

%.o: %.c
	@echo [CC]  $<
	@$(CROSS)gcc $(OPT) $(COPT) $(OPT.$<) $(INC) $(DEFINES) $(DEFINES.$<) $(WARN) -MMD -c -o $@ $<

%.o: %.S
	@echo [AS]  $<
	@$(CROSS)gcc $(OPT) $(OPT.$<) $(INC) $(DEFINES) $(DEFINES.$<) $(WARN) -MMD -c -o $@ $<

main.aout : $(OBJS)
	@echo [LD] $@
	@$(CROSS)gcc -o $@ -T $(LDSCRIPT) -nostartfiles $^ -L ../MyLib -lMyLib -lgcc -Wl,-Map=$(basename $@).map
	@$(CROSS)objdump -d $@ >$(basename $@).list

%.bin : %.aout
	@echo [OBJCOPY] $<
	@$(CROSS)objcopy -O binary --only-section=.text --only-section=.data $< $@


clean:
	@echo [CLEAN]
	@rm -f *.o *.d *.aout *.bin *.map *.list $(OBJS)
