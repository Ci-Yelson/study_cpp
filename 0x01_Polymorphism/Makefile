.PHONY: build
build:
# -g: generate debug information
	g++ vtable.cc -o vtable -g

.PHONY: run
run:
	./vtable

.PHONY: clean
clean:
	rm -f vtable

.PHONY: objdump-sections
objdump-sections:
	objdump -h vtable



