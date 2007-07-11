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
    xmlNodePtr node;
    xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr xpathObj;     
} fxml_data_t;

static void fastxml_data_mark( fxml_data_t *data );
static void fastxml_data_free( fxml_data_t *data );

static VALUE rb_cFastXmlDoc;
static VALUE rb_cFastXmlNode;

static VALUE fastxml_doc_initialize(VALUE self, VALUE data);
static VALUE fastxml_doc_search(VALUE self, VALUE raw_xpath);
static VALUE fastxml_doc_to_s(VALUE self);
static VALUE fastxml_doc_root(VALUE self);
static VALUE fastxml_node_initialize(VALUE self);
static VALUE fastxml_node_search(VALUE self, VALUE raw_xpath);
static VALUE fastxml_node_name(VALUE self);
static VALUE fastxml_node_value(VALUE self);
static VALUE fastxml_node_value_set(VALUE self, VALUE new_val);
static VALUE fastxml_node_innerxml(VALUE self);
static VALUE fastxml_node_to_s(VALUE self);


static VALUE fastxml_nodeset_to_obj(xmlXPathObjectPtr xpath_obj, fxml_data_t *data);

void Init_fastxml()
{
    xmlXPathInit();
    VALUE rb_mFastXml = rb_define_module( "FastXml" );
    rb_define_const( rb_mFastXml, "VERSION", rb_str_new2( "0.1" ) );
    rb_cFastXmlDoc = rb_define_class_under( rb_mFastXml, "Doc", rb_cObject );        
    rb_cFastXmlNode = rb_define_class_under( rb_mFastXml, "Node", rb_cObject );

    /* Doc */
    rb_include_module( rb_cFastXmlDoc, rb_mEnumerable );
    rb_define_method( rb_cFastXmlDoc, "initialize", fastxml_doc_initialize, 1 );
    rb_define_method( rb_cFastXmlDoc, "search", fastxml_doc_search, 1 );
    rb_define_method( rb_cFastXmlDoc, "to_s", fastxml_doc_to_s, 0 );
    rb_define_method( rb_cFastXmlDoc, "root", fastxml_doc_root, 0 );
    
    /* Node */
    rb_include_module( rb_cFastXmlNode, rb_mEnumerable );
    rb_define_method( rb_cFastXmlNode, "initialize", fastxml_node_initialize, 0 );
    rb_define_method( rb_cFastXmlNode, "search", fastxml_node_search, 1 );
    rb_define_method( rb_cFastXmlNode, "to_s", fastxml_node_to_s, 0 );
    rb_define_method( rb_cFastXmlNode, "name", fastxml_node_name, 0 );
    rb_define_method( rb_cFastXmlNode, "content", fastxml_node_value, 0 );
    rb_define_method( rb_cFastXmlNode, "content=", fastxml_node_value_set, 1 );
    rb_define_method( rb_cFastXmlNode, "inner_xml", fastxml_node_innerxml, 0 );
}


/* {{{ fastxml_node 
 */

static VALUE fastxml_node_initialize(VALUE self)
{
    return Qnil;
}

static VALUE fastxml_node_innerxml(VALUE self)
{
    return Qnil;
}

static VALUE fastxml_node_name(VALUE self)
{
    VALUE ret, dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_node" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 

    if (data->node->name == NULL)
        return Qnil;

    ret = rb_str_new2( (const char*)data->node->name );

    return ret;
}

static VALUE fastxml_node_value_set(VALUE self, VALUE new_val)
{
    VALUE dv, val_s;
    fxml_data_t *data;
    xmlChar *ents, *spec;


    val_s = rb_obj_as_string( new_val );
    dv = rb_iv_get( self, "@lxml_node" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 

    ents = xmlEncodeEntitiesReentrant( data->doc, (const xmlChar*)StringValuePtr(val_s) );
    spec = xmlEncodeSpecialChars( data->doc, ents );

    printf("setting chars: %s\n", spec);
    xmlNodeSetContent( data->node, spec );
    xmlFree( ents );


    return new_val;
}

static VALUE fastxml_node_value(VALUE self)
{
    VALUE ret, dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_node" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 

    if (data->node->content == NULL)
        return Qnil;

    ret = rb_str_new2( (const char*)data->node->content );

    return ret;
}

static VALUE fastxml_node_to_s(VALUE self)
{
    return Qnil;
}

static VALUE fastxml_node_search(VALUE self, VALUE raw_xpath)
{
    return Qnil;
}


/* }}} fastxml_node
 */

/* {{{ fastxml_doc 
 */

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
        rb_raise( rb_eRuntimeError, "unable to create xpath context" );
        return Qnil;
    }

    xpath_obj = xmlXPathEvalExpression( xpath_expr, xpath_ctx );
    if(xpath_obj == NULL) {
        rb_raise( rb_eRuntimeError, "unable to evaluate xpath expression" );
        xmlXPathFreeContext( xpath_ctx ); 
        return Qnil;
    }    

    ret = fastxml_nodeset_to_obj( xpath_obj, data );

    xmlXPathFreeObject( xpath_obj );
    xmlXPathFreeContext( xpath_ctx ); 
    printf("done\n");

    return ret;
}

static VALUE fastxml_nodeset_to_obj(xmlXPathObjectPtr xpath_obj, fxml_data_t *data)
{
    VALUE ret, dv_chld, new_tmp;
    int size, i;
    xmlNodeSetPtr nodes = xpath_obj->nodesetval;
    xmlNodePtr cur;
    fxml_data_t *chld;

    ret = rb_ary_new();
    size = (nodes) ? nodes->nodeNr : 0;
    printf("size: %d\n", size);

    for (i = 0; i < size; i++) {
        cur = nodes->nodeTab[i];
        printf( "checking node: %s type: %d\n", cur->name, cur->type );
        if (cur->type == XML_ELEMENT_NODE)
            continue;


        chld = ALLOC(fxml_data_t);
        memset( chld, 0, sizeof(chld) );
        chld->node = cur;
        chld->doc = data->doc; 

        new_tmp = rb_class_new_instance( 0, 0, rb_cFastXmlNode ); 
        dv_chld = Data_Wrap_Struct( rb_cObject, fastxml_data_mark, fastxml_data_free, chld );
        rb_iv_set( new_tmp, "@lxml_node", dv_chld );
        rb_ary_push( ret, new_tmp );
    }

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

static VALUE fastxml_doc_root(VALUE self)
{
    VALUE ret, dv, dv_chld;
    fxml_data_t *data;
    fxml_data_t *chld;
    xmlNodePtr root;

    dv = rb_iv_get( self, "@lxml_doc" );
    Data_Get_Struct( dv, fxml_data_t, data );

    root = xmlDocGetRootElement( data->doc );
    
    chld = ALLOC(fxml_data_t);
    memset( chld, 0, sizeof(chld) );
    chld->node = root;
    chld->doc = data->doc; 

    ret = rb_class_new_instance( 0, 0, rb_cFastXmlNode ); 
    dv_chld = Data_Wrap_Struct( rb_cObject, fastxml_data_mark, fastxml_data_free, chld );
    rb_iv_set( ret, "@lxml_node", dv_chld );

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

    dv = Data_Wrap_Struct( rb_cObject, fastxml_data_mark, fastxml_data_free, data );
    rb_iv_set(self, "@lxml_doc", dv );

    return self;
}


/* }}} fastxml_doc 
 */

static void fastxml_data_mark( fxml_data_t *data )
{
    /* do nothing */
}

static void fastxml_data_free( fxml_data_t *data )
{
    printf("attempting to free\n");
    if (data != NULL)
    {
        if (data->doc != NULL && data->node == NULL)
            xmlFreeDoc( data->doc );

//        if (data->node != NULL)
//            xmlFreeNode( data->node );
        // the doc free will cleanup the nodes

        data->doc = NULL;
        data->node = NULL;
        free(data);
    }
    data = NULL;
}

static VALUE fastxml_data_alloc( VALUE klass )
{
    //return Data_Wrap_Struct(klass, file_mark, file_free, f);
    return Qnil;
}
