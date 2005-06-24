# ---------------------------------------------
# Makefile for the Taco Java RPC library 
# ---------------------------------------------

all:	jar

classList:
	find . \( -name "*.java" \) | grep -v remotetea | grep -v examples | grep -v tests | grep -v "oncrpc/ant" | grep -v CVS > ./.classes

jdoc:	classList
	if ! test -d doc ; then mkdir doc ; fi
	touch doc/package-list
	javadoc -d doc @.classes
	@echo "-------------------------------------"
	@echo `wc -l .classes | cut -f1 -d' '` classes documented.
	@echo "-------------------------------------"

jar:	classList
	if ! test -d lib ; then mkdir lib ; fi
	javac -classpath remotetea.jar -d lib @.classes
	jar cvf Taco.jar -C lib .

test:   
