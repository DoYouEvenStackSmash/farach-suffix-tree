CXX := g++
TARGET := target
OBJ := odd_tree_driver.o compacted_trie.o basic_blocks.o

DEP := $(OBJ:.o=.d)

CXXFLAGS = -Wall -g -MMD
