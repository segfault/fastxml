/*
 *  $Id$
 */
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

static VALUE rb_cFastXmlData;
static VALUE fastxml_doc_initialize(VALUE self, VALUE data);
static VALUE fastxml_doc_search(VALUE self, VALUE raw_xpath);
static void fastxml_data_mark( fxml_data_t *data );
static void fastxml_data_free( fxml_data_t *data );
static VALUE fastxml_doc_to_s(VALUE self);

void Init_fastxml()
{
    VALUE rb_mFastXml = rb_define_module( "FastXml" );
    VALUE rb_cFastXmlDoc = rb_define_class_under( rb_mFastXml, "Doc", rb_cObject );        
    VALUE rb_cFastXmlData = rb_define_class_under( rb_mFastXml, "Data", rb_cObject );
    rb_include_module( rb_cFastXmlDoc, rb_mEnumerable );
    rb_define_method( rb_cFastXmlDoc, "initialize", fastxml_doc_initialize, 1 );
    rb_define_method( rb_cFastXmlDoc, "search", fastxml_doc_search, 1 );
    rb_define_method( rb_cFastXmlDoc, "to_s", fastxml_doc_to_s, 0 );
    
}

static VALUE fastxml_doc_search(VALUE self, VALUE raw_xpath)
{
    VALUE ret, dv, xpath_s;
    xmlXPathContextPtr xpath_ctx; 
    xmlXPathObjectPtr xpath_obj;     
    fxml_data_t *data;
    xmlChar *xpath_expr;

    if (NIL_P(raw_xpath)) {
        printf("got nil\n");
        rb_raise(rb_eArgError, "nil passed as xpath");
        return Qnil;
    }

    xpath_s = rb_obj_as_string( raw_xpath );
    xpath_expr = (xmlChar*)StringValuePtr( xpath_s );
    printf("got xpath: %s\n", xpath_expr);

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 

    xpath_ctx = xmlXPathNewContext( data->doc );
    if (xpath_ctx == NULL) {
        fprintf(stderr,"Error: unable to create new XPath context\n");
        return Qnil;
    }

    xpath_obj = xmlXPathEvalExpression( xpath_expr, xpath_ctx );
    if(xpath_obj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpath_expr);
        xmlXPathFreeContext( xpath_ctx ); 
        return Qnil;
    }    

    xmlXPathFreeObject( xpath_obj );
    xmlXPathFreeContext( xpath_ctx ); 
    printf("done");

    return ret;
}

static VALUE fastxml_doc_to_s(VALUE self)
{
    VALUE ret, dv;
    xmlChar *xs;
    fxml_data_t *data;
    int xs_len;

    dv = rb_iv_get( self, "@lxml_doc" );
    Data_Get_Struct( dv, fxml_data_t, data );

    xmlDocDumpFormatMemory( data->doc, &xs, &xs_len, 0 );

    ret = rb_str_new( (const char*)xs, xs_len );
    xmlFree( xs );

    return ret; 
}

static VALUE fastxml_doc_initialize(VALUE self, VALUE xml_doc_str)
{
    VALUE data_s, dv;
    char *cstr;
    fxml_data_t *data;

    if (NIL_P(xml_doc_str)) {
        rb_raise(rb_eArgError, "nil passed as xml document");
        return Qnil;
    }


    data_s = rb_obj_as_string( xml_doc_str );
    rb_iv_set( self, "@raw_data", data_s );

    cstr = StringValuePtr( data_s );
    printf( "cstr: %s\n", cstr );

    xmlInitParser();
    data = ALLOC(fxml_data_t);
    memset( data, 0, sizeof(data) );

    data->doc = xmlReadMemory( cstr, RSTRING(data_s)->len, "noname.xml", NULL, 0 );
    if (data->doc == NULL) {
        rb_raise( rb_eRuntimeError, "Failed to parse document" );
	    return Qnil;
    }
    
    xmlCleanupParser();

    dv = Data_Wrap_Struct( rb_cFastXmlData, fastxml_data_mark, fastxml_data_free, data );
    rb_iv_set(self, "@lxml_doc", dv );

    return self;
}

static void fastxml_data_mark( fxml_data_t *data )
{
    /* do nothing */
}

static void fastxml_data_free( fxml_data_t *data )
{
    printf("attempting to free\n");
    if (data != 0)
    {
        if (data->doc != 0)
            xmlFreeDoc( data->doc );

        free(data);
    }
    data = 0;
}

static VALUE fastxml_data_alloc( VALUE klass )
{
    //return Data_Wrap_Struct(klass, file_mark, file_free, f);
    return Qnil;
}
