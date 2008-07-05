/* Provides common headers and definitions for the FastXml ruby extension
 *
 *  Please see the LICENSE file for copyright, licensing and distribution information
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
#include <libxml/HTMLparser.h> 

#ifndef XML_WITH_TREE
	#define XML_WITH_TREE 2
	#define XML_WITH_XPATH 16
#endif

#ifndef HTML_PARSE_RECOVER
    #define HTML_PARSE_RECOVER 1
#endif

typedef struct {
    xmlDocPtr doc;
    xmlNodePtr node;
	xmlNodePtr list;
    xmlXPathObjectPtr xpath_obj;     
	xsltStylesheetPtr xslt;
	int list_len;
} fxml_data_t;


#ifndef fastxml_c
RUBY_EXTERN VALUE rb_mFastXml;
RUBY_EXTERN VALUE rb_cFastXmlDoc;
RUBY_EXTERN VALUE rb_cFastXmlNode;
RUBY_EXTERN VALUE rb_cFastXmlNodeList;
RUBY_EXTERN VALUE rb_cFastXmlAttrList;

RUBY_EXTERN VALUE rb_sValidateDtd;
RUBY_EXTERN VALUE rb_sForgivingParse;
RUBY_EXTERN VALUE rb_sHtmlParse;

RUBY_EXTERN ID s_readlines;
RUBY_EXTERN ID s_to_s;

RUBY_EXTERN VALUE fastxml_xpath_search(VALUE self, VALUE raw_xpath, VALUE blk);
RUBY_EXTERN VALUE fastxml_raw_node_to_obj(xmlNodePtr cur);
RUBY_EXTERN VALUE fastxml_nodeset_to_obj(xmlXPathObjectPtr xpath_obj, fxml_data_t *data);
RUBY_EXTERN VALUE fastxml_nodelist_to_obj(xmlNodePtr root, int len);
RUBY_EXTERN void fastxml_data_mark( fxml_data_t *data );
RUBY_EXTERN void fastxml_data_free( fxml_data_t *data );
#endif
#endif /* fastxml_h */
