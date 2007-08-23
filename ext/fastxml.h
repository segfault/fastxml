/*
 *  $Id$
 */

#ifndef fastxml_h
#define fastxml_h

#include <ruby.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>

#ifndef XML_WITH_TREE
	#define XML_WITH_TREE 2
	#define XML_WITH_XPATH 16
#endif

typedef struct {
    xmlDocPtr doc;
    xmlNodePtr node;
	xmlNodePtr list;
    xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr xpathObj;     
	xsltStylesheetPtr xslt;
	int listLen;
	short listType;
} fxml_data_t;


#ifndef fastxml_c
RUBY_EXTERN VALUE rb_cFastXmlDoc;
RUBY_EXTERN VALUE rb_cFastXmlNode;

RUBY_EXTERN VALUE fastxml_xpath_search(VALUE self, VALUE raw_xpath);
RUBY_EXTERN VALUE fastxml_raw_node_to_obj(xmlNodePtr cur);
RUBY_EXTERN VALUE fastxml_nodeset_to_obj(xmlXPathObjectPtr xpath_obj, fxml_data_t *data);
RUBY_EXTERN VALUE fastxml_nodelist_to_obj(xmlNodePtr root, int len);
RUBY_EXTERN void fastxml_data_mark( fxml_data_t *data );
RUBY_EXTERN void fastxml_data_free( fxml_data_t *data );
#endif
#endif /* fastxml_h */