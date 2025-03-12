
#Configuring the Makefile
MMCU = atmega328p

#Source Folder
source = ./SourceCode

#Compiling the C script with avr-gcc.
AVRDUDE = avrdude
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRCONFIG = $(CC) -g -Os -mmcu=$(MMCU) -DF_CPU=12000000L -c

all: 
	$(AVRCONFIG) $(source)/main.c -o main.o
	$(AVRCONFIG) $(source)/CRC/CRC.c -o CRC.o
	$(AVRCONFIG) $(source)/UART/UART.c -o UART.o
	$(AVRCONFIG) $(source)/IRQ/ISR.c -o ISR.o
	$(AVRCONFIG) $(source)/Layer1/physicalLayer.c -o physicalLayer.o
	$(AVRCONFIG) $(source)/Layer2/dataLinkLayer.c -o dataLinkLayer.o
	$(AVRCONFIG) $(source)/Layer3/networkLayer.c -o networkLayer.o

	$(CC) -mmcu=$(MMCU) -o main.elf main.o CRC.o UART.o ISR.o physicalLayer.o dataLinkLayer.o networkLayer.o

	$(OBJCOPY) -O ihex main.elf main.hex

	sudo $(AVRDUDE) -c gpio -p $(MMCU) -v -U flash:w:main.hex

clean:
	rm -rf main.o  main.elf  main.hex CRC.o UART.o ISR.o physicalLayer.o dataLinkLayer.o networkLayer.o ./DoxygenDocuments

doc:
	doxygen DoxygenConfiguration
