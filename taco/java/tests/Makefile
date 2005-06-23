# ---------------------------------------------
# Makefile for the Taco Java RPC library 
# ---------------------------------------------

all:	compil

classList:
	find . \( -name "*.java" \) > ./.classes

compil:	classList
	javac -classpath ../Taco.jar @.classes

run:
	java -DNETHOST=$(NETHOST) -classpath ../Taco.jar:. Client1
