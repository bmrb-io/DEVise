main: main.o sec.o cal.o
	g++ -o main main.o sec.o cal.o

crsp_index: crsp_index.o sec.o
	g++ -o crsp_index crsp_index.o sec.o

crsp_index.o: crsp_index.C sec.h
	g++ -g -c crsp_index.C

main.o: main.C
	g++ -g -c main.C

sec.o: sec.C sec.h
	g++ -g -c sec.C

cal.o: cal.C cal.h
	g++ -g -c cal.C
