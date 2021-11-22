CC			:= gcc
EXE			:= lab1
C_FILES		:= lab1.c
O_FILES		:= lab1.o

.PHONY: all

all: $(O_FILES)
	@$(CC) -o $(EXE) $(O_FILES)

%.o:
	@$(CC) -c $(C_FILES)

clean:
	@rm -rf $(O_FILES) $(EXE)
