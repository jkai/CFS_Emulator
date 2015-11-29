all: CFS_Emulator

CFS_Emulator: CFS_Emulator.c 
	gcc -pthread CFS_Emulator.c -o CFS_Emulator

clean:
	rm CFS_Emulator