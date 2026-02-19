CC = g++
EXE = exe
flags = -Iinclude -Wall -Wextra -Werror -std=c++20 -MMD -MP

all: setup move
	@$(MAKE) $(EXE) --no-print-directory
	@echo "Compilado e linkado com sucesso"

setup:
	@mkdir -p include src obj

move:
	@ls *.hpp >/dev/null 2>&1 && mv *.hpp include/ || true
	@ls *.cpp >/dev/null 2>&1 && mv *.cpp src/ || true

arqs = $(wildcard src/*.cpp)
arqsO = $(arqs:src/%.cpp=obj/%.o)
DEPS = $(arqsO:.o=.d)

$(EXE): $(arqsO)
	@$(CC) $(arqsO) -o $(EXE)

obj/%.o: src/%.cpp
	@$(CC) $(flags) -c $< -o $@
-include $(DEPS)

clean:
	@rm -rf obj $(EXE)