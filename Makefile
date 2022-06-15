all: libs unittests

libs:
	make -C src

unittests:
	make -C tests

clean:
	make -C src clean
	make -C tests clean
