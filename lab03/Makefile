mainFileName = main
files = DeviceType
allFiles = $(mainFileName) $(files)
filesObj = $(addsuffix .o, $(allFiles))
filesH = $(addsuffix .h, $(files))

all: cleanExe $(mainFileName)

# Execution
$(mainFileName): $(filesObj)
	g++ -O2 $^ -o $@

# g++ $(CFLAGS) -O3  $^ -o $@

# Compilation
%.o: %.cpp $(filesH)
	g++ -O2 -c $<

# g++ -c $< $(CFLAGS)

# Utils
clean:
	rm -f *.o $(mainFileName)

cleanExe:
	rm -f $(mainFileName)
