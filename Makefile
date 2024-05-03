all: 
	gcc -o exec fabrica.c -lpthread

run: all
	@echo -e "\nExecucao 1 - Argumentos (25, 2, 2, 3, 10, 2, 5)\n"
	./exec 25 2 2 3 10 2 5
	@echo -e "\nExecucao 2 - Argumentos (50, 1, 1, 1, 5, 1, 2)\n"
	./exec 50 1 1 1 5 1 2

run_aleatorio: all
	@echo -e "\nExecutando o programa com argumentos de entrada aletorios...\n"
	./exec $$(($$RANDOM % 200 + 1)) $$(($$RANDOM % 40 + 1)) $$(($$RANDOM % 10 + 1)) $$(($$RANDOM % 10 + 1)) $$(($$RANDOM % 50 + 1)) $$(($$RANDOM % 10 + 1)) $$(($$RANDOM % 10 + 1))
	
