require 'mkmf'
ext_name = 'fastxml'
dir_config ext_name
find_header( 'libxml/tree.h', "/usr/include/libxml2", "/usr/local/include/libxml2", "/opt/local/include/libxml2"  )
find_header( 'libxslt/xslt.h', "/usr/include/libxslt", "/usr/local/include/libxslt", "/opt/local/include/libxslt"  )
find_library( 'xml2', "xmlInitParser", "/usr/lib", "/usr/local/lib", "/opt/local/lib" )
have_library( 'xml2', 'xmlInitParser', 'libxml/parser.h' )
find_library( 'xslt', "xmlInitParser", "/usr/lib", "/usr/local/lib", "/opt/local/lib" )
have_library( 'xslt', 'xsltParseStylesheetFile', 'libxslt/xslt.h' )
$LDFLAGS << ' %s' % (`xml2-config --libs`).chomp
$CFLAGS << ' -Wall %s' % (`xml2-config --cflags`).chomp 

create_makefile ext_name
