/* Document-class: FastXml
 *  Provides the base namespace for the FastXml classes
 *   # FastXml::Doc
 *   # FastXml::Node
 *   # FastXml::NodeList
 *   # FastXml::AttrList
 */
// Please see the LICENSE file for copyright, licensing and distribution information

#define fastxml_c 1
#include "fastxml.h"
#include "fastxml_node.h"
#include "fastxml_doc.h"
#include "fastxml_nodelist.h"
#include "fastxml_attrlist.h"

#ifdef _WIN32
__declspec(dllexport) 
#endif


VALUE rb_mFastXml;
VALUE rb_cFastXmlDoc;
VALUE rb_cFastXmlNode;
VALUE rb_cFastXmlNodeList;
VALUE rb_cFastXmlAttrList;
VALUE rb_sValidateDtd;
VALUE rb_sForgivingParse;
VALUE rb_sHtmlParse;
ID s_readlines;
ID s_to_s;

void Init_fastxml()
{	
    if (xmlHasFeature(XML_WITH_TREE) == 0)
        rb_raise( rb_eRuntimeError, "libxml not built with tree support" );

    if (xmlHasFeature(XML_WITH_XPATH) == 0)
        rb_raise( rb_eRuntimeError, "libxml not built with xpath support" );

	s_readlines = rb_intern("readlines");
	s_to_s = rb_intern("to_s");

    xmlInitParser();
    xmlXPathInit();
	xsltInit();
    rb_mFastXml = rb_define_module( "FastXml" );
    rb_define_const( rb_mFastXml, "LIBXML_VERSION", rb_str_new2( LIBXML_DOTTED_VERSION ) );

    /* setting symbols */
    rb_sValidateDtd = ID2SYM( rb_intern("validate") );
    rb_sForgivingParse = ID2SYM( rb_intern("forgiving") );
    rb_sHtmlParse = ID2SYM( rb_intern("html") );
    
	Init_fastxml_doc();       /* Doc */    
	Init_fastxml_node();      /* Node */
	Init_fastxml_nodelist();  /* NodeList */
	Init_fastxml_attrlist();  /* AttrList */
	
	/* pull in the ruby side of things */
	rb_require( "fastxml/fastxml_lib" );      // ruby-side methods for the FastXml classes
	rb_require( "fastxml/fastxml_helpers" );  // FastXml and FastHtml methods
}




void fastxml_data_mark( fxml_data_t *data )
{
    /* do nothing */
}

void fastxml_data_free( fxml_data_t *data )
{
    if (data != NULL)
    {
		if (data->xpath_obj != NULL) 
			xmlXPathFreeObject( data->xpath_obj );			
	
		if (data->xslt != NULL)
			xsltFreeStylesheet( data->xslt );
	
        if (data->doc != NULL && data->node == NULL && data->list == NULL && data->xpath_obj == NULL) 
            xmlFreeDoc( data->doc );

        // the doc free will cleanup the nodes

		data->xpath_obj = NULL;
		data->list = NULL;
        data->doc = NULL;
		data->xslt = NULL;
		data->node = NULL;
        free( data );
    }
    data = NULL;
}

VALUE fastxml_data_alloc( VALUE klass )
{
    return Qnil;
}

VALUE fastxml_raw_node_to_my_obj(xmlNodePtr cur, fxml_data_t *chld)
{
    VALUE dv_chld, new_tmp;
    chld->node = cur;
    chld->doc = cur->doc; 

    new_tmp = rb_class_new_instance( 0, 0, rb_cFastXmlNode ); 
    dv_chld = Data_Wrap_Struct( rb_cObject, fastxml_data_mark, fastxml_data_free, chld );
    rb_iv_set( new_tmp, "@lxml_doc", dv_chld );	
	
	return new_tmp;	
}

VALUE fastxml_raw_node_to_obj(xmlNodePtr cur)
{
    fxml_data_t *chld = ALLOC(fxml_data_t);
    memset( chld, 0, sizeof(fxml_data_t) );
	return fastxml_raw_node_to_my_obj( cur, chld );
}

VALUE fastxml_nodelist_to_obj(xmlNodePtr root, int len)
{
    VALUE ret, dv_chld;
    xmlNodePtr cur = root;
	fxml_data_t *ndlst = ALLOC(fxml_data_t);
	memset( ndlst, 0, sizeof(fxml_data_t) );
	
	ndlst->list_len = len;
	ndlst->list = cur;
	ret = rb_class_new_instance( 0, 0, rb_cFastXmlNodeList ); 
    dv_chld = Data_Wrap_Struct( rb_cObject, fastxml_data_mark, fastxml_data_free, ndlst );
    rb_iv_set( ret, "@lxml_doc", dv_chld );
   
	return ret;
}

VALUE fastxml_nodeset_to_obj(xmlXPathObjectPtr raw_xpath_obj, fxml_data_t *data)
{
    VALUE ret, dv_chld;
	fxml_data_t *ndlst = ALLOC(fxml_data_t);
	memset( ndlst, 0, sizeof(fxml_data_t) );
	
	ndlst->xpath_obj = raw_xpath_obj;
	ndlst->list_len = raw_xpath_obj->nodesetval->nodeNr;
	ret = rb_class_new_instance( 0, 0, rb_cFastXmlNodeList ); 
    dv_chld = Data_Wrap_Struct( rb_cObject, fastxml_data_mark, fastxml_data_free, ndlst );
    rb_iv_set( ret, "@lxml_doc", dv_chld );
   
	return ret;
}

VALUE munge_xpath_namespace( VALUE orig_expr, xmlChar *root_ns )
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
		
		if (RSTRING_LEN(path_bit) > 0) {
			str_idx = rb_funcall( path_bit, rb_intern( "index" ), 1, ns_indic );
			if (str_idx == Qnil || str_idx == Qfalse) // didn't find the :, so it looks like we don't have a namespace
				path_bit = rb_str_plus( ns_prefix, path_bit );
		}	
		
		rb_ary_push( ret_ary, path_bit );
    }
	
	return rb_ary_join( ret_ary, slash );
}


/**
 *  fastxml_xpath_search provides a common xpath search function for
 * the libraries bits (node, doc). it handles mangling non-namespaced
 * xpath queries into something libxml will play nice with
 */
VALUE fastxml_xpath_search(VALUE self, VALUE raw_xpath, VALUE blk)
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
		root_ns = (xmlChar*)root->ns->prefix;
		if (root_ns == NULL) 
			root_ns = (xmlChar*)"__myFunkyLittleRootNsNotToBeUseByAnyoneElseIHope__"; 
            // alternatives? how do other xpath processors handle root/default namespaces?

		xmlXPathRegisterNs( xpath_ctx, root_ns, root->ns->href );
		// need to update the xpath expression
		xpath_s = munge_xpath_namespace( xpath_s, root_ns );
		xpath_ctx->nsNr++;
	}
	
	xpath_expr = (xmlChar*)RSTRING_PTR(xpath_s);
	xpath_xpr = xmlXPathCompile( xpath_expr );
	if (xpath_xpr == NULL) {
		xmlXPathFreeContext( xpath_ctx );
		xmlFree( ns_list );		
		rb_raise( rb_eRuntimeError, "unable to evaluate xpath expression" );
	}	

	xpath_obj = xmlXPathCompiledEval( xpath_xpr, xpath_ctx );
    if (xpath_obj == NULL) {
        rb_raise( rb_eRuntimeError, "unable to evaluate xpath expression" );
		xmlXPathFreeCompExpr( xpath_xpr );
        xmlXPathFreeContext( xpath_ctx ); 
		xmlFree( ns_list );
        return Qnil;
    }    

    ret = fastxml_nodeset_to_obj( xpath_obj, data );

	xmlFree( ns_list );
	xmlXPathFreeCompExpr( xpath_xpr );
    xmlXPathFreeContext( xpath_ctx ); 

    return ret;	
}

