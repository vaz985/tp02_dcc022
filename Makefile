CC=g++
ROUTER_FLAGS=-O1	-Wall	-std=c++14	-pthread -fsanitize=address	-fno-omit-frame-pointer	-g
BIN=./bin



all:	prep	router.o	tools.o table.o	route.o
		$(CC)	$(BIN)/router.o	$(BIN)/tools.o	$(BIN)/table.o	$(BIN)/route.o	-o	router	$(ROUTER_FLAGS)

prep:
		mkdir	-p	$(BIN)

router.o:
	$(CC)	-c	router.cpp	-o	$(BIN)/router.o	$(ROUTER_FLAGS)

tools.o:
	$(CC)	-c	tools.cpp		-o	$(BIN)/tools.o	$(ROUTER_FLAGS)

table.o:
	$(CC)	-c	table.cpp	-o	$(BIN)/table.o	$(ROUTER_FLAGS)

route.o:
	$(CC)	-c	route.cpp	-o	$(BIN)/route.o	$(ROUTER_FLAGS)

clean:
	rm	-f	router
	rm	-f	$(BIN)/*.o
	rm	-r	$(BIN)
