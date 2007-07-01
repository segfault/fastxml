require 'mkmf'
ext_name = 'fastxml'
dir_config ext_name
find_header( 'libxml/tree.h', "/usr/include/libxml2", "/usr/local/include/libxml2", "/opt/local/include/libxml2"  )
find_library( 'xml2', "xmlInitParser", "/usr/lib", "/usr/local/lib", "/opt/local/lib" )
have_library( 'xml2', 'xmlInitParser', 'libxml/parser.h' )
create_makefile ext_name
