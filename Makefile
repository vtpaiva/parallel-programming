all:
	gcc -o exec fabrica_entrada.c -lpthread
run:
	./exec 20 5 3 1 8 4 6

aleatorio:
	@rm -f aleatorio.in
	@echo $$((RANDOM % 200 + 1)) $$((RANDOM % 40 + 1)) $$((RANDOM % 10 + 1)) $$((RANDOM % 10 + 1)) $$((RANDOM % 50 + 1)) $$((RANDOM % 10 + 1)) $$((RANDOM % 10 + 1)) >> aleatorio.in
	gcc -o exec fabrica.c -lpthread
	./exec < aleatorio.in
