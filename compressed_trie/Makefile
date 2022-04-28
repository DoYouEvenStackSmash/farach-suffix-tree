# *********************

CXX = g++
CXXFLAGS = -Wall -g
# ****************

EXECUTABLE = driver
# ****************

driver: compressed_trie.o
	$(CXX) $(CXXFLAGS) -o driver compressed_trie.o

compressed_trie.o: compressed_trie.cc
	$(CXX) $(CXXFLAGS) -c compressed_trie.cc

clean:
	/bin/rm -fv *.o $(EXECUTABLE)
