mainFileName = Lab1
files = Task SchedulingDiscipline SchedulingDisciplinePER SchedulingDisciplineFIFO
allFiles = $(mainFileName) $(files)
filesObj = $(addsuffix .o, $(allFiles))
filesH = $(addsuffix .h, $(files))

all: cleanExe $(mainFileName)

# Execution
$(mainFileName): $(filesObj)
	g++ $(CFLAGS) $^ -o $@

# Compilation
%.o: %.cpp $(filesH)
	g++ -c $< $(CFLAGS)

# Utils
clean:
	rm -f *.o Lab1

cleanExe:
	rm -f Lab1
