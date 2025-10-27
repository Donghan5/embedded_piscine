CC = avr-gcc

OBJCOPY = avr-objcopy

FLASHER = avrdude

MCU = atmega328p

# Cpu frequency
F_CPU = 16000000UL

PROGRAMMER = arduino

PORT = /dev/ttyUSB0

# Transfer speed
BAUDRATE = 115200

#============================
# File Setting
#============================
TARGET = main
BUILD_DIR := ./build
SRC = ./src/$(TARGET).c
ELF = $(BUILD_DIR)/$(TARGET).elf
BIN = $(BUILD_DIR)/$(TARGET).bin
HEX = $(BUILD_DIR)/$(TARGET).hex


CFLAGS = -Wall -Werror -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)

#=============================
# Rule
# Build: .c -> .elf (master) -> .bin(binary) -> .hex(transfer)
#=============================
all: hex flash
	@echo "--- [All] Build and Flash Complete ---"

# Convert machine to hex (to understand memory address)
hex: $(HEX)
	@echo "--- [Hex] Target $(HEX) is ready ---"

flash: $(HEX)
	@echo "--- [Flash] Flashing $(HEX) to $(MCU) ---"
	$(FLASHER) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUDRATE) -U flash:w:$(HEX):i
	@echo "--- [Flash] Flashing complete ---"

$(HEX): $(BIN)
	@echo "Generating $(HEX) from $(BIN)..."
	$(OBJCOPY) -I binary -O ihex $(BIN) $(HEX)

$(BIN): $(ELF)
	@echo "Generating $(BIN) from $(ELF)..."
	$(OBJCOPY) -O binary -R .eeprom $(ELF) $(BIN)


$(ELF): $(SRC) $(BUILD_DIR)
	@echo "Compiling $(SRC) with F_CPU=$(F_CPU)..."
	$(CC) $(CFLAGS) -o $(ELF) $(SRC)

$(BUILD_DIR):
	@echo "Creating build directory..."
	mkdir -p $(BUILD_DIR)

clean:
	@echo "Cleaning up generated files..."
	rm -rf $(BUILD_DIR)
	@echo "Cleanup complete."

.PHONY: all hex flash clean