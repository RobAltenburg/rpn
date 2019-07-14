all: rpn.scm
	csc -optimize-level 3 rpn.scm

install: rpn
	cp rpn /usr/local/bin/
