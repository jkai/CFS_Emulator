all: CFS_Emulator

CFS_Emulator: CFS_Emulator.o cpu_queue.o
	gcc -pthread -o CFS_Emulator CFS_Emulator.o cpu_queue.o

CFS_Emulator.o: CFS_Emulator.c cpu_queue.h Common.h
	gcc -pthread -c CFS_Emulator.c

cpu_queue.o: cpu_queue.c cpu_queue.h Common.h
	gcc -c cpu_queue.c

clean:
	rm *.o CFS_Emulator
