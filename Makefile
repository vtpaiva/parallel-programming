all:
	gcc -o exec factory.c -lpthread

run:
	@echo -e "\nExample - Arguments (25, 5, 2, 2, 10, 2, 3)\n"
	./exec 25 5 2 2 10 2 3

examples: all
	@echo -e "\nExample 1 - Arguments (25, 5, 1, 1, 10, 12, 8)\n"
	./exec 25 5 1 1 10 12 8
	@echo -e "\nExample 2 - Arguments (15, 2, 2, 2, 7, 3, 2)\n"
	./exec 15 2 2 2 7 3 2
	@echo -e "\nExample 3 - Arguments (50, 15, 1, 1, 5, 5, 5)\n"
	./exec 50 15 1 1 5 5 5
	@echo -e "\nExample 4 - Arguments (44, 7, 2, 3, 5, 10, 10)\n"
	./exec 44 7 2 3 5 10 10
	@echo -e "\nExample 5 - Arguments (60, 19, 2, 1, 1, 12, 3)\n"
	./exec 60 19 2 1 1 12 3

clean:
	rm -f exec
