all: $(builddir)/beatdown

$(builddir)/beatdown: $(srcdir)/beatdown.vala
	valac --thread -o $@ $(srcdir)/beatdown.vala --pkg catalina-1.0 --pkg gio-2.0 --vapidir $(top_srcdir)/bindings/vala

clean:
	rm -rf $(builddir)/beatdown
