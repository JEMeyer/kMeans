all:
	g++ program.cpp kMeans.cpp dataPoint.cpp -std=gnu++0x
clean:
	rm -f a.out 