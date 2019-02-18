# variable
OBJECTS = API.o Attribute.o BufferManager.o CatalogManager.o \
          Condition.o IndexManager.o Interpreter.o RecordManager.o

# executable
all: minisql
.PHONY: all

minisql: main.o $(OBJECTS)
	g++ -g -o $@ main.o $(OBJECTS)

API.o: API.cpp API.h
Attribute.o: Attribute.cpp Attribute.h
BufferManager.o: BufferManager.cpp BufferManager.h
CatalogManager.o: CatalogManager.cpp CatalogManager.h
Condition.o: Condition.cpp Condition.h
IndexManager.o: IndexManager.cpp IndexManager.h
Interpreter.o: Interpreter.cpp Interpreter.h
RecordManager.o: RecordManager.cpp RecordManager.h

.PHONY: clean
clean:
	rm -f *.o a.out
