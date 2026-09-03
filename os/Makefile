ifeq ($(OS),Windows_NT)
    RM = del
    EXE = app.exe
else
    RM = rm -f
    EXE = app
endif

all: app
	./app
	make clean


app: app.cpp buddy.hpp worst-fit.hpp circular-fit.hpp
	g++ app.cpp buddy.hpp worst-fit.hpp circular-fit.hpp -o app


clean:	
	$(RM) $(EXE)