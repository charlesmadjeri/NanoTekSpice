SRC		:=	$(shell find ./src -type f -name '*.cpp')
TEST_SRC := $(shell find -type f -name '*.cpp' ! -name 'main.cpp')

OBJ		 = $(SRC:.cpp=.o)
TEST_OBJ = $(TEST_SRC:.cpp=.o)

CXX		 = g++
CFLAGS	 = -std=c++20 -Wall -Wextra
V_FLAG   = --leak-check=full --show-leak-kinds=all --track-origins=yes
EXE		 = nanotekspice
TEST_EXE = unit_tests
SAN		 = -g3
COV		 = --coverage

all:	$(EXE)

%.o:	%.cpp
	$(CXX) -o $@ -c $< $(CFLAGS) $(SAN) $(COV)

$(EXE): $(OBJ)
	$(CXX) -o $(EXE) $^ $(COV)

$(TEST_EXE): $(TEST_OBJ)
	$(CXX) -o $(TEST_EXE) $^ -lcriterion $(COV)

clean:
	@rm -rf $(OBJ)
	@rm -rf $(TEST_OBJ)
	@rm -rf vgcore*
	@find . -type f -name '*.gcda' -delete
	@find . -type f -name '*.gcno' -delete

fclean:	clean
	@rm -rf $(EXE)
	@rm -rf $(TEST_EXE)

re:	fclean all

val:
	make re && valgrind ./$(EXE)

val_full:
	make re && valgrind $(V_FLAG) ./$(EXE)

test: $(TEST_EXE)
	./$(TEST_EXE) --verbose

tests_run: fclean test	# for mouli
	gcovr --exclude-directories test

.PHONY: all clean fclean re val val_full test tests_run
