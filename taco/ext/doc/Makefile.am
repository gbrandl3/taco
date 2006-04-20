EXTRA_DIST = Doxyfile

doc:
	rm -rf @PACKAGE@.tag html
	doxygen Doxyfile

install-doc:
	$(mkinstalldirs) $(DESTDIR)/@docdir@
	cp -R @PACKAGE@.tag html $(DESTDIR)/@docdir@

uninstall-doc:
	rm -rf $(DESTDIR)/@docdir@/@PACKAGE@.tag $(DESTDIR)/@docdir@/html
