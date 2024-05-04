all: 
	gcc -o exec fabrica.c -lpthread

run:
	@echo -e "\nExemplo - Argumentos (30, 12, 3, 2, 10, 7, 3)\n"
	./exec 25 5 1 1 10 2 3

clean:
	rm exec

exemplos: 
	all
	@echo -e "\nExecucao 1 - Argumentos (25, 5, 1, 1, 10, 12, 8)\n"
	./exec 25 5 1 1 10 12 8
	@echo -e "\nExecucao 2 - Argumentos (50, 15, 1, 1, 5, 5, 2)\n"
	./exec 50 15 1 1 5 5 2
	@echo -e "\nExecucao 3 - Argumentos (44, 7, 2, 3, 5, 10, 10)\n"
	./exec 44 7 2 3 5 10 10
	@echo -e "\nExecucao 4 - Argumentos (15, 2, 3, 2, 7, 3, 2)\n"
	./exec 15 2 3 2 7 3 2
	@echo -e "\nExecucao 5 - Argumentos (60, 19, 2, 1, 1, 12, 2)\n"
	./exec 60 19 2 1 1 12 2
