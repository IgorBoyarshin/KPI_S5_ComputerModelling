# all: Lab1
all: cleanExe Lab1

# Execution
Lab1: Lab1.o SchedulingDiscipline.o SchedulingDisciplinePER.o SchedulingDisciplineFIFO.o Task.o
	g++ $(CFLAGS) -o Lab1 Lab1.o SchedulingDiscipline.o SchedulingDisciplinePER.o SchedulingDisciplineFIFO.o Task.o

Lab1.o: Lab1.cpp
	g++ $(CFLAGS) -c Lab1.cpp

SchedulingDiscipline.o: SchedulingDiscipline.cpp
	g++ $(CFLAGS) -c SchedulingDiscipline.cpp

SchedulingDisciplinePER.o: SchedulingDisciplinePER.cpp
	g++ $(CFLAGS) -c SchedulingDisciplinePER.cpp

SchedulingDisciplineFIFO.o: SchedulingDisciplineFIFO.cpp
	g++ $(CFLAGS) -c SchedulingDisciplineFIFO.cpp

Task.o: Task.cpp
	g++ $(CFLAGS) -c Task.cpp


clean:
	rm -f *.o Lab1

cleanExe:
	rm -f Lab1
