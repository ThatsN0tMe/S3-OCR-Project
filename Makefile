CC := gcc
CFLAGS := -Wall -Wextra
TARGET := Afflelou
OBJS := afflelou.o Functions.o Solver/solver.o Pretreatment/pretreatment.o Pretreatment/image_history.o Rotate/rotate.o Rotate/AutoRotate.o Interface/Interface.o Interface/FileSetup.o Detection/Cutting.o Detection/LetterDetection.o Detection/WordDetection.o Detection/LineDetection.o Detection/GridDetection.o Detection/DrawShapes.o Detection/Detection.o Neural-Network/neural-net/network_functions.o Neural-Network/neural-net/Network.o Neural-Network/shared/arr_helpers.o Neural-Network/shared/math_helpers.o Neural-Network/convert.o
SPEED := -Ofast -march=native -mtune=native -mavx2 

SDL_LIBS := -lSDL2 -lSDL2_image -lm

GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS := $(shell pkg-config --libs gtk+-3.0)

%.o: %.c
	$(CC) $(CFLAGS) $(SPEED) $(GTK_CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(SPEED) -o $(TARGET) $(OBJS) $(GTK_LIBS) $(SDL_LIBS)
	./Afflelou

debug: CFLAGS += -fsanitize=address -g
debug: LDFLAGS += -fsanitize=address -g
debug: $(TARGET)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)