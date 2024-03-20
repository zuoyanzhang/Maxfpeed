TAR = bin/mpfrCodeGene.exe
CPP = detectModule/mpfrCodeGene/src/*.cpp
CC := g++
Include = -lm -lmpfr -I ./detectModule/
$(TAR) : $(CPP)
	$(CC) $(CPP) -o $(TAR) $(Include)
.PHONY:
clean:
	rm $(TAR)