/*
 *  $Id$
 */

#ifndef fastxml_h
#define fastxml_h

#include "ruby.h"

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
    xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr xpathObj;     
	xsltStylesheetPtr xslt;
} fxml_data_t;

static VALUE rb_cFastXmlDoc;
static VALUE rb_cFastXmlNode;

#ifndef fastxml_c
extern void fastxml_data_mark( fxml_data_t *data );
extern void fastxml_data_free( fxml_data_t *data );
#endif
#endif /* fastxml_h */
