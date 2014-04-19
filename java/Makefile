# ---------------------------------------------
# Makefile for the Taco Java RPC library 
# ---------------------------------------------

PROJECT = taco-java
VERSION = 3.99.70

JAVAC = javac
JAVADOC = javadoc
JAR = jar
PREFIX ?= /usr/local
CLASSPATH = /usr/share/java/oncrpc.jar

DISTNAME=$(PROJECT)-$(VERSION)


all: jar doc

classes: classes-stamp
classes-stamp:
	@find . \( -name "*.java" \) | grep -v remotetea | grep -v examples | grep -v tests | grep -v "oncrpc/ant" | grep -v CVS > classes
	@touch classes-stamp


doc: doc-stamp
doc-stamp: classes-stamp
	@if ! test -d doc ; then mkdir doc ; fi
	@if test -f remotetea.jar ; then\
		$(JAVADOC) -classpath remotetea.jar -d doc @classes;\
		echo $(JAVADOC) -classpath remotetea.jar -d doc @classes;\
	else\
		$(JAVADOC) -classpath $(CLASSPATH) -d doc @classes;\
		echo $(JAVADOC) -classpath $(CLASSPATH) -d doc @classes;\
	fi
	@echo "-------------------------------------"
	@echo `wc -l classes | cut -f1 -d' '` classes documented.
	@echo "-------------------------------------"
	@touch doc-stamp

jar: jar-stamp
jar-stamp: classes-stamp
	@if ! test -d lib ; then mkdir lib ; fi
	@if test -f remotetea.jar ; then\
		$(JAVAC) -classpath remotetea.jar -d lib @classes;\
		echo $(JAVAC) -classpath remotetea.jar -d lib @classes;\
	else\
		$(JAVAC) -classpath $(CLASSPATH) -d lib @classes;\
		echo $(JAVAC) -classpath $(CLASSPATH) -d lib @classes;\
	fi
	$(JAR) cvf Taco.jar -C lib .
	@touch jar-stamp

clean:
	@rm -rf classes classes-stamp jar-stamp doc-stamp lib doc Taco.jar doc/package-list

install: all
	@install -d $(DESTDIR)/$(PREFIX)/share/java
	install -m 644 Taco.jar $(DESTDIR)/$(PREFIX)/share/java
	@install -d $(DESTDIR)/$(PREFIX)/share/doc/$(PROJECT)
	cp -rf doc/* $(DESTDIR)/$(PREFIX)/share/doc/$(PROJECT)

uninstall:
	@rm -rf $(DESTDIR)/$(PREFIX)/share/java/Taco.jar
	@rm -rf $(DESTDIR)/$(PREFIX)/share/doc/$(PROJECT)

dist:
	@if ! test -d $(DISTNAME) ; then mkdir $(DISTNAME) ; fi
	@cp -r Makefile src tests xdrarrays $(DISTNAME)
	find $(DISTNAME) \( -name .svn -o -name .git -o -name CVS -o -name .cvsignore -o -name \*.class -o -name remotetea.jar \) -print0 | xargs -0 /bin/rm -rf
	tar -cjf $(DISTNAME).tar.bz2 $(DISTNAME)
	@/bin/rm -rf $(DISTNAME)

distclean: clean

.PHONY: clean install uninstall dist distclean
