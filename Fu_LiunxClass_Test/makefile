.PHONY:all
all:fifoRead fifoWrite 

fifoWrite:fifoWrite.c
	gcc -o $@ $^ 

fifoRead:fifoRead.c
	gcc -o $@ $^ 

.PHONY:clean
clean:
	rm -rf fifoRead fifoWrite