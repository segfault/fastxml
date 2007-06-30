#include "ruby.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

typedef struct {
    xmlDocPtr doc;
    xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr xpathObj;     
} fxml_data_t;


void Init_FastXml()
{
    VALUE rb_mFastXml = rb_define_module("FastXml");
    VALUE rb_cFastXmlDoc = rb_define_class_under(rb_mFastXml, "Doc", rb_cObject);        
    rb_include_module(rb_cFastXmlDoc, rb_mEnumerable);
    rb_define_method(rb_cFastXmlDoc, "initialize", fastxml_doc_initialize, -1);
    
}

static VALUE fastxml_doc_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE data_s = rb_any_to_s( data );
    rb_iv_set(self, "@raw_data", data);

    char *cstr = StringValuePtr( data_s );
    printf( "cstr: %s\n", cstr );
    xmlInitParser();


    xmlCleanupParser();

    return self;
}
