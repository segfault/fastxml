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

static void fastxml_data_mark( fxml_data_t *data );
static void fastxml_data_free( fxml_data_t *data );

static VALUE rb_cFastXmlDoc;
static VALUE rb_cFastXmlNode;

static VALUE fastxml_doc_initialize(VALUE self, VALUE data);
static VALUE fastxml_doc_search(VALUE self, VALUE raw_xpath);
static VALUE fastxml_doc_to_s(VALUE self);
static VALUE fastxml_doc_root(VALUE self);
static VALUE fastxml_doc_transform(VALUE self, VALUE xform);
static VALUE fastxml_doc_stylesheet(VALUE self);
static VALUE fastxml_doc_stylesheet_set(VALUE self, VALUE style);
static VALUE fastxml_doc_children(VALUE self);

VALUE fastxml_xpath_search(VALUE self, VALUE raw_xpath);

static VALUE fastxml_node_initialize(VALUE self);
static VALUE fastxml_node_search(VALUE self, VALUE raw_xpath);
static VALUE fastxml_node_name(VALUE self);
static VALUE fastxml_node_value(VALUE self);
static VALUE fastxml_node_value_set(VALUE self, VALUE new_val);
static VALUE fastxml_node_innerxml(VALUE self);
static VALUE fastxml_node_to_s(VALUE self);
static VALUE fastxml_node_xpath(VALUE self);
static VALUE fastxml_node_attr(VALUE self, VALUE attr_name);
static VALUE fastxml_node_children(VALUE self);
static VALUE fastxml_node_next(VALUE self);
static VALUE fastxml_node_prev(VALUE self);
static VALUE fastxml_node_parent(VALUE self);

static VALUE fastxml_raw_node_to_obj(xmlNodePtr cur);
static VALUE fastxml_nodeset_to_obj(xmlXPathObjectPtr xpath_obj, fxml_data_t *data);
static VALUE fastxml_nodelist_to_obj(xmlNodePtr root);

void Init_fastxml()
{
    if (xmlHasFeature(XML_WITH_TREE) == 0)
        rb_raise( rb_eRuntimeError, "libxml not built with tree support" );

    if (xmlHasFeature(XML_WITH_XPATH) == 0)
        rb_raise( rb_eRuntimeError, "libxml not built with xpath support" );

    xmlInitParser();
    xmlXPathInit();
    VALUE rb_mFastXml = rb_define_module( "FastXml" );
    //rb_define_const( rb_mFastXml, "VERSION", rb_str_new2( "0.1" ) );
    rb_cFastXmlDoc = rb_define_class_under( rb_mFastXml, "Doc", rb_cObject );        
    rb_cFastXmlNode = rb_define_class_under( rb_mFastXml, "Node", rb_cObject );

    /* Doc */
    //rb_include_module( rb_cFastXmlDoc, rb_mEnumerable );
    rb_define_method( rb_cFastXmlDoc, "initialize", fastxml_doc_initialize, 1 );
    rb_define_method( rb_cFastXmlDoc, "search", fastxml_doc_search, 1 );
    rb_define_method( rb_cFastXmlDoc, "to_s", fastxml_doc_to_s, 0 );
    rb_define_method( rb_cFastXmlDoc, "root", fastxml_doc_root, 0 );
	rb_define_method( rb_cFastXmlDoc, "transform", fastxml_doc_transform, 1 );
	rb_define_method( rb_cFastXmlDoc, "stylesheet=", fastxml_doc_stylesheet_set, 1 );
	rb_define_method( rb_cFastXmlDoc, "stylesheet", fastxml_doc_stylesheet, 0 );
	rb_define_method( rb_cFastXmlDoc, "children", fastxml_doc_children, 0 );
    
    /* Node */
    //rb_include_module( rb_cFastXmlNode, rb_mEnumerable );
    rb_define_method( rb_cFastXmlNode, "initialize", fastxml_node_initialize, 0 );
    rb_define_method( rb_cFastXmlNode, "search", fastxml_node_search, 1 );
    rb_define_method( rb_cFastXmlNode, "to_s", fastxml_node_to_s, 0 );
    rb_define_method( rb_cFastXmlNode, "name", fastxml_node_name, 0 );
    rb_define_method( rb_cFastXmlNode, "content", fastxml_node_value, 0 );
    rb_define_method( rb_cFastXmlNode, "content=", fastxml_node_value_set, 1 );
    rb_define_method( rb_cFastXmlNode, "inner_xml", fastxml_node_innerxml, 0 );
	rb_define_method( rb_cFastXmlNode, "xpath", fastxml_node_xpath, 0 );
	rb_define_method( rb_cFastXmlNode, "attr", fastxml_node_attr, 1 );
	rb_define_method( rb_cFastXmlNode, "children", fastxml_node_children, 0 );
	rb_define_method( rb_cFastXmlNode, "next", fastxml_node_next, 0 );	
	rb_define_method( rb_cFastXmlNode, "prev", fastxml_node_prev, 0 );	
	rb_define_method( rb_cFastXmlNode, "parent", fastxml_node_parent, 0 );	
	
	rb_require( "lib/fastxml_lib" );
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

static VALUE fastxml_node_next(VALUE self)
{
	VALUE dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->node == NULL || (data->node != NULL && data->node->next == NULL))
		return Qnil;
	
	return fastxml_raw_node_to_obj( data->node->next );
}

static VALUE fastxml_node_prev(VALUE self)
{
	VALUE dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->node == NULL || (data->node != NULL && data->node->prev == NULL))
		return Qnil;
	
	return fastxml_raw_node_to_obj( data->node->prev );
}

static VALUE fastxml_node_parent(VALUE self)
{
	VALUE dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->node == NULL || (data->node != NULL && data->node->parent == NULL))
		return Qnil;
	
	return fastxml_raw_node_to_obj( data->node->parent );
}

static VALUE fastxml_node_children(VALUE self)
{
	VALUE dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->node == NULL || (data->node != NULL && data->node->children == NULL))
		return Qnil;
	
	return fastxml_nodelist_to_obj( data->node->children );
}

static VALUE fastxml_node_name(VALUE self)
{
    VALUE ret, dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 

    if (data->node == NULL || (data->node != NULL && data->node->name == NULL))
        return Qnil;

    ret = rb_str_new2( (const char*)data->node->name );

    return ret;
}

static VALUE fastxml_node_attr(VALUE self, VALUE attr_name)
{
	VALUE ret, dv;
	fxml_data_t *data;
	xmlChar *raw_ret, *name_str;
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );
	
	name_str = (xmlChar*)StringValuePtr( attr_name );
	raw_ret = xmlGetProp( data->node, name_str );
	if (raw_ret == NULL)
		return Qnil;
		
	ret = rb_str_new2( (const char*)raw_ret );
	xmlFree( raw_ret );
	
	return ret;
}

static VALUE fastxml_node_xpath(VALUE self)
{
	VALUE ret, dv;
	fxml_data_t *data;
	xmlChar *raw_ret;
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );
	
	raw_ret = xmlGetNodePath( data->node );
	if (raw_ret == NULL)
		return Qnil;
	
	ret = rb_str_new2( (const char*)raw_ret );
	xmlFree( raw_ret );
	
	return ret;
}

static VALUE fastxml_node_value_set(VALUE self, VALUE new_val)
{
    VALUE dv, val_s;
    fxml_data_t *data;
    xmlChar *ents, *spec;


    val_s = rb_obj_as_string( new_val );
    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 

    ents = xmlEncodeEntitiesReentrant( data->doc, (const xmlChar*)StringValuePtr(val_s) );
    spec = xmlEncodeSpecialChars( data->doc, ents );

   // printf("setting chars: %s\n", spec);
    xmlNodeSetContent( data->node, spec );
    xmlFree( ents );

    return new_val;
}

static VALUE fastxml_node_value(VALUE self)
{
    VALUE ret, dv;
    fxml_data_t *data;
	xmlChar *cont;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 

	cont = xmlNodeGetContent( data->node );

    if (cont == NULL)
        return Qnil;

    ret = rb_str_new2( (const char*)cont );

    return ret;
}

static VALUE fastxml_node_to_s(VALUE self)
{
    VALUE ret, dv;
    fxml_data_t *data;
    xmlBufferPtr buf;

    dv = rb_iv_get( self, "@lxml_doc" );
    Data_Get_Struct( dv, fxml_data_t, data );

    buf = xmlBufferCreate();
    ret = Qnil;

    if (xmlNodeDump(buf, data->doc, data->node, 0, 0) != -1) 
        ret = rb_str_new( (const char*)buf->content, buf->use );

    xmlBufferFree( buf );
    return ret;
}

static VALUE fastxml_node_search(VALUE self, VALUE raw_xpath)
{
    return fastxml_xpath_search( self, raw_xpath );
}


/* }}} fastxml_node
 */

/* {{{ fastxml_doc 
 */
static VALUE fastxml_doc_children(VALUE self)
{
	VALUE dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->doc->children == NULL)
		return Qnil;
	
	return fastxml_nodelist_to_obj( data->doc->children );
}

static VALUE fastxml_doc_stylesheet(VALUE self)
{
	return rb_iv_get( self, "@lxml_style" );
}

static VALUE fastxml_doc_stylesheet_set(VALUE self, VALUE style)
{
	VALUE ret, dv, data_s, xslt_doc;
	fxml_data_t *data;
	char *cstr;		

    xslt_doc = rb_class_new_instance(1, &style, rb_cFastXmlDoc );
	
    dv = rb_iv_get( xslt_doc, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );
	data->xslt = xsltParseStylesheetDoc( data->doc );
	rb_iv_set( self, "@lxml_style", xslt_doc );
	
	return Qnil;	
}

static VALUE fastxml_doc_transform(VALUE self, VALUE xform)
{
	VALUE ret, dv, xform_dv, ret_str, ret_dv;
	fxml_data_t *my_data, *xf_data, *ret_data;
	xmlDocPtr ret_doc;

	if (xform == Qnil)
		return Qnil;
	
    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, my_data );
	xform_dv = rb_iv_get( xform, "@lxml_doc" );
	Data_Get_Struct( xform_dv, fxml_data_t, xf_data );
	
	if (xf_data->xslt == NULL)
		return Qnil;

	ret_doc = xsltApplyStylesheet( xf_data->xslt, my_data->doc, NULL );
	ret_str = rb_str_new2( "<shouldNeverBeSeen/>" );
	ret = rb_class_new_instance( 1, &ret_str, rb_cFastXmlDoc );
	ret_dv = rb_iv_get( ret, "@lxml_doc" );
	Data_Get_Struct( ret_dv, fxml_data_t, ret_data );
	xmlFree( ret_data->doc );
	ret_data->doc = ret_doc;
	
	return ret;
}

static VALUE fastxml_doc_search(VALUE self, VALUE raw_xpath)
{
	return fastxml_xpath_search( self, raw_xpath );
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

    return fastxml_raw_node_to_obj( root );
}

static VALUE fastxml_doc_initialize(VALUE self, VALUE xml_doc_str)
{
    VALUE data_s, dv;
    char *cstr;
    fxml_data_t *data;
    int parser_opts = XML_PARSE_NOERROR & XML_PARSE_NOWARNING;
    int parse_dtd = XML_PARSE_DTDLOAD & XML_PARSE_DTDATTR & XML_PARSE_DTDVALID;
    int parse_forgiving = XML_PARSE_RECOVER;

    if (NIL_P(xml_doc_str)) {
        rb_raise(rb_eArgError, "nil passed as xml document");
        return Qnil;
    }


    data_s = rb_obj_as_string( xml_doc_str );
    rb_iv_set( self, "@raw_data", data_s );

    data = ALLOC(fxml_data_t);
    memset( data, (int)NULL, sizeof(fxml_data_t) );

    data->doc = xmlReadMemory( RSTRING(data_s)->ptr, RSTRING(data_s)->len, 
                               "noname.xml", NULL, parser_opts );
    // if we're mallformed we might want to use xmlRecoverMemcory(char*, int)
    if (data->doc == NULL) {
        rb_raise( rb_eRuntimeError, "Failed to parse document" );
	    return Qnil;
    }
    
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
    if (data != NULL)
    {
        if (data->doc != NULL && data->node == NULL)
            xmlFreeDoc( data->doc );

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

static VALUE fastxml_raw_node_to_my_obj(xmlNodePtr cur, fxml_data_t *chld)
{
    VALUE dv_chld, new_tmp;
    chld->node = cur;
    chld->doc = cur->doc; 

    new_tmp = rb_class_new_instance( 0, 0, rb_cFastXmlNode ); 
    dv_chld = Data_Wrap_Struct( rb_cObject, fastxml_data_mark, fastxml_data_free, chld );
    rb_iv_set( new_tmp, "@lxml_doc", dv_chld );	
	
	return new_tmp;	
}

static VALUE fastxml_raw_node_to_obj(xmlNodePtr cur)
{
    fxml_data_t *chld = ALLOC(fxml_data_t);
    memset( chld, (int)NULL, sizeof(fxml_data_t) );
	return fastxml_raw_node_to_my_obj( cur, chld );
}

static VALUE fastxml_nodelist_to_obj(xmlNodePtr root)
{
    VALUE ret, dv_chld, new_tmp;
    int size, i;
    xmlNodePtr cur = root;

    ret = rb_ary_new();
	while (cur != NULL)
	{        
        rb_ary_push( ret, fastxml_raw_node_to_obj( cur ) );
		cur = cur->next;
	}

	return ret;
}

static VALUE fastxml_nodeset_to_obj(xmlXPathObjectPtr xpath_obj, fxml_data_t *data)
{
    VALUE ret = rb_ary_new();
    int size, i;
    xmlNodeSetPtr nodes = xpath_obj->nodesetval;
    xmlNodePtr cur;
    fxml_data_t **chld = NULL;

    size = (nodes) ? nodes->nodeNr : 0;

    for (i = 0; i < size; i++) {
        cur = nodes->nodeTab[i];
        rb_ary_push( ret, fastxml_raw_node_to_obj( cur ) );
    }

    return ret;
}

static VALUE munge_xpath_namespace( VALUE orig_expr, xmlChar *root_ns )
{
	VALUE path_bits = rb_str_split( orig_expr, "/" );
	VALUE ns_prefix = rb_str_new2( (const char*)root_ns );
	VALUE ns_indic = rb_str_new2( ":" );
	VALUE slash = rb_str_new2( "/" );
	VALUE path_bit, str_idx;
	VALUE ret_ary = rb_ary_new();
	long i;
	
	rb_str_append( ns_prefix, ns_indic );
    for (i=0; i<RARRAY(path_bits)->len; i++) {
        path_bit = RARRAY(path_bits)->ptr[i];
		
		if (RSTRING(path_bit)->len > 0) {
			str_idx = rb_funcall( path_bit, rb_intern( "index" ), 1, ns_indic );
			if (str_idx == Qnil || str_idx == Qfalse) // didn't find the :, so it looks like we don't have a namespace
				path_bit = rb_str_plus( ns_prefix, path_bit );
		}	
		
		rb_ary_push( ret_ary, path_bit );
    }
	
	return rb_ary_join( ret_ary, slash );
}

VALUE fastxml_xpath_search(VALUE self, VALUE raw_xpath)
{
    VALUE ret, dv, xpath_s;
	xmlXPathCompExprPtr xpath_xpr;
    xmlXPathContextPtr xpath_ctx; 
    xmlXPathObjectPtr xpath_obj;     
    fxml_data_t *data;
    xmlChar *xpath_expr;
	xmlNodePtr root = NULL;
	xmlNsPtr *ns_list = NULL;
	xmlNsPtr *cur_ns = NULL;
	xmlChar *root_ns = NULL;
	int ns_cnt = 0;

    if (NIL_P(raw_xpath)) 
        rb_raise(rb_eArgError, "nil passed as xpath");

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data ); 

    xpath_ctx = xmlXPathNewContext( data->doc );
    if (xpath_ctx == NULL) 
        rb_raise( rb_eRuntimeError, "unable to create xpath context" );

	root = data->node;
	if (root == NULL)
		root = xmlDocGetRootElement( data->doc );
		
	xpath_ctx->node = root;
	cur_ns = ns_list = xmlGetNsList( data->doc, root );
	while (cur_ns != NULL && (*cur_ns) != NULL) { 
		xmlXPathRegisterNs( xpath_ctx, (*cur_ns)->prefix, (*cur_ns)->href );
		cur_ns++;
	}

    if (ns_list != NULL) {
	    xpath_ctx->namespaces = ns_list;
	    xpath_ctx->nsNr = ns_cnt;
    }
	
	xpath_s = rb_obj_as_string( raw_xpath );
	if (root->ns != NULL) { // we have a base namespace, this is going to get "interesting"
		root_ns = root->ns->prefix;
		if (root_ns == NULL) 
			root_ns = (xmlChar*)"myFunkyLittleRootNsNotToBeUseByAnyoneElseIHope"; 
            // alternatives? how do other xpath processors handle root/default namespaces?

		xmlXPathRegisterNs( xpath_ctx, root_ns, root->ns->href );
		// need to update the xpath expression
		xpath_s = munge_xpath_namespace( xpath_s, root_ns );
		xpath_ctx->nsNr++;
	}
	
	xpath_expr = (xmlChar*)RSTRING(xpath_s)->ptr;
	xpath_xpr = xmlXPathCompile( xpath_expr );
	if (xpath_xpr == NULL) {
		xmlXPathFreeContext( xpath_ctx );
		xmlFree( ns_list );		
		rb_raise( rb_eRuntimeError, "unable to evaluate xpath expression" );
	}	

	xpath_obj = xmlXPathCompiledEval( xpath_xpr, xpath_ctx );
    if(xpath_obj == NULL) {
        rb_raise( rb_eRuntimeError, "unable to evaluate xpath expression" );
		xmlXPathFreeCompExpr( xpath_xpr );
        xmlXPathFreeContext( xpath_ctx ); 
		xmlFree( ns_list );
        return Qnil;
    }    

    ret = fastxml_nodeset_to_obj( xpath_obj, data );

	xmlFree( ns_list );
	xmlXPathFreeCompExpr( xpath_xpr );
    xmlXPathFreeObject( xpath_obj );
    xmlXPathFreeContext( xpath_ctx ); 

    return ret;	
}

