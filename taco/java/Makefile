# ---------------------------------------------
# Makefile for the Taco Java RPC library 
# ---------------------------------------------

all:	jar

classList:
	find . \( -name "*.java" \) | grep -v remotetea | grep -v examples | grep -v tests | grep -v "oncrpc/ant" > ./.classes

jdoc:	classList
	touch doc/package-list
	javadoc -d doc @.classes
	@echo "-------------------------------------"
	@echo `wc -l classes` documented.
	@echo "-------------------------------------"

jar:	classList
	javac -classpath remotetea.jar -d lib @.classes
	jar cvf Taco.jar -C lib .

test:   
