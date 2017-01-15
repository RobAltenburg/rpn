all: rpn.scm
	csc -optimize-level 2 rpn.scm

install: rpn
	cp rpn /usr/local/bin/
