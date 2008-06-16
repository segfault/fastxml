/*
 * Document-class: FastXml::Doc
 * Wraps a libxml xml document in memory, providing methods
 * to modify and extract data from the document.
 *
 * Example:
 *  doc = FastXml( docfile ) # parse the docfile into memory
 *  puts "first node"
 *  puts "name = value (text node inside of the element)"
 *  puts "%s = %s" % [ doc.children.first.name, doc.children.first.content ]
 *  doc.children.each do |node|
 *  	puts node.inspect
 *  end
 */
// Please see the LICENSE file for licensing and distribution information

#include "fastxml.h"
#include "fastxml_node.h"
#include "fastxml_doc.h"
#include "fastxml_nodelist.h"

/* {{{ fastxml_doc 
 */
void Init_fastxml_doc()
{
	#ifdef RDOC_SHOULD_BE_SMARTER__THIS_IS_NEVER_RUN
    rb_mFastXml = rb_define_module( "FastXml" );
	#endif 
	rb_cFastXmlDoc = rb_define_class_under( rb_mFastXml, "Doc", rb_cObject );  
	
	rb_define_method( rb_cFastXmlDoc, "initialize", fastxml_doc_initialize, -1 );
    rb_define_method( rb_cFastXmlDoc, "search", fastxml_doc_search, 1 );
    rb_define_method( rb_cFastXmlDoc, "to_s", fastxml_doc_to_s, 0 );
    rb_define_method( rb_cFastXmlDoc, "root", fastxml_doc_root, 0 ); 
	rb_define_method( rb_cFastXmlDoc, "transform", fastxml_doc_transform, 1 );
	rb_define_method( rb_cFastXmlDoc, "stylesheet=", fastxml_doc_stylesheet_set, 1 );
	rb_define_method( rb_cFastXmlDoc, "stylesheet", fastxml_doc_stylesheet, 0 );
	rb_define_method( rb_cFastXmlDoc, "children", fastxml_doc_children, 0 );	
	rb_define_method( rb_cFastXmlDoc, "inspect", fastxml_doc_inspect, 0 );
}


/* Returns a friendly summary of the doc
 *
 */
VALUE fastxml_doc_inspect(VALUE self)
{
    VALUE *argv;
    argv = ALLOCA_N( VALUE, 3 );
    argv[0] = rb_str_new2( "#<%s:0x%x %s>" );
    argv[1] = CLASS_OF( self );
    argv[2] = rb_obj_id( self );
	argv[3] = fastxml_doc_to_s( self );

    return rb_f_sprintf( 4, argv );
}

/* Returns an Array containing FastXml::Node representations
 * of the child elements of the doc. They are provided in the order they are found.
 */ 
VALUE fastxml_doc_children(VALUE self)
{
	VALUE dv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

	if (data->doc->children == NULL)
		return Qnil;
	
	return fastxml_nodelist_to_obj( data->doc->children, -1 );
}

/* Returns the FastXml::Doc that's defined as the xsl for this document
 */
VALUE fastxml_doc_stylesheet(VALUE self)
{
	return rb_iv_get( self, "@lxml_style" );
}

/* 
 * call-seq:
 *   doc.stylesheet = FastXml::Doc.new( open( 'my.xsl' ) )
 */
VALUE fastxml_doc_stylesheet_set(VALUE self, VALUE style)
{
	VALUE dv, xslt_doc;
	fxml_data_t *data;

    xslt_doc = rb_class_new_instance(1, &style, rb_cFastXmlDoc );
	
    dv = rb_iv_get( xslt_doc, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );
	data->xslt = xsltParseStylesheetDoc( data->doc );
	rb_iv_set( self, "@lxml_style", xslt_doc );
	
	return Qnil;	
}

/* Applys an XSLT to the target FastXml::Doc.
 * Returns the resulting FastXml::Doc
 * 
 * call-seq:
 *   doc.transform FastXml::Doc.new( open( 'my.xslt' ) ) 
 */
VALUE fastxml_doc_transform(VALUE self, VALUE xform)
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

	ret_doc = (xmlDocPtr)xsltApplyStylesheet( xf_data->xslt, my_data->doc, NULL );
	ret_str = rb_str_new2( "<shouldNeverBeSeen/>" );
	ret = rb_class_new_instance( 1, &ret_str, rb_cFastXmlDoc );
	ret_dv = rb_iv_get( ret, "@lxml_doc" );
	Data_Get_Struct( ret_dv, fxml_data_t, ret_data );
	xmlFree( ret_data->doc );
	ret_data->doc = ret_doc;
	
	return ret;
}

/* Evaluates an xpath query and returns a list of nodes
 * that match.
 *
 * call-seq:
 *   doc.search( "//nodes" ).each { |n| puts n.inspect }
 */
VALUE fastxml_doc_search(VALUE self, VALUE raw_xpath, VALUE blk)
{
	return fastxml_xpath_search( self, raw_xpath, blk );
}

/* Returns the string representation of the xml document.
 * Basically a wrapper around xmlDocDumpFormatMemory
 *
 * call-seq:
 *   puts doc.to_s
 */
VALUE fastxml_doc_to_s(VALUE self)
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

/* Returns the FastXml::Node object representing the root element of
 * the target document
 *
 * call-seq:
 *   puts doc.root.name
 */
VALUE fastxml_doc_root(VALUE self)
{
    VALUE dv;
    fxml_data_t *data;
    xmlNodePtr root;

    dv = rb_iv_get( self, "@lxml_doc" );
    Data_Get_Struct( dv, fxml_data_t, data );

    root = xmlDocGetRootElement( data->doc );

    return fastxml_raw_node_to_obj( root );
}

/* Parse an input string/array/stringio object into a FastXml::Doc object.
 * 
 * call-seq:
 *   doc = FastXml::Doc.new( open( "test.xml" ) )
 *   doc = FastXml::Doc.new( open( "test.xml" ).readlines )
 *   doc = FastXml::Doc.new( "<test><node>taco</node></test>" )
 *   doc = FastXml::Doc.new( open( "test.xml" ), { :forgiving => true } ) # turn on the forgiving/liberal libxml parser
 *   doc = FastXml::Doc.new( open( "test.xml" ), { :validate_dtd => true } ) # turn on strict dtd parsing and loading, invalid xml will cause an exception.
 *   FastXml::Doc.new( open( "test.xml" ) ) do |doc|
 *     doc.children.each { |child_node| puts child_node.name }
 *   end
 */
VALUE fastxml_doc_initialize(int argc, VALUE* argv, VALUE self)
{
    VALUE data_s, dv, lines, xml_doc_str, opts, blk;
    fxml_data_t *data;
    int parser_opts = XML_PARSE_NOERROR | XML_PARSE_NOWARNING;
    short html_parser = 0;

    if (rb_scan_args( argc, argv, "11&", &xml_doc_str, &opts, &blk ) == 0)
        return Qnil; // error state

    if (NIL_P(xml_doc_str)) {
        rb_raise(rb_eArgError, "nil passed as xml document");
        return Qnil;
    }

    if (opts != Qnil) {
        if (rb_hash_aref(opts, rb_sValidateDtd) == Qtrue) {
            parser_opts = parser_opts | XML_PARSE_DTDLOAD | XML_PARSE_DTDATTR | XML_PARSE_DTDVALID;
			rb_iv_set( self, "@validate_dtd", Qtrue );
        }

        if (rb_hash_aref(opts, rb_sForgivingParse) == Qtrue) {
            parser_opts = parser_opts | XML_PARSE_RECOVER;
			rb_iv_set( self, "@forgiving", Qtrue );
		}

        if (rb_hash_aref(opts, rb_sHtmlParse) == Qtrue) {
            html_parser = 1;
        }
    }

	if (rb_respond_to( xml_doc_str, s_readlines )) {
		lines = rb_funcall( xml_doc_str, s_readlines, 0 );
		data_s = rb_funcall( lines, s_to_s, 0 );
	}
	else
    	data_s = rb_obj_as_string( xml_doc_str );
    
	rb_iv_set( self, "@raw_data", data_s );

    data = ALLOC(fxml_data_t);
    memset( data, (int)NULL, sizeof(fxml_data_t) );

    if (html_parser == 0)
        data->doc = xmlReadMemory( RSTRING(data_s)->ptr, RSTRING(data_s)->len, 
                               "noname.xml", NULL, parser_opts );
    else
        data->doc = htmlReadMemory( RSTRING(data_s)->ptr, RSTRING(data_s)->len,
                                    "noname.html", NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING );

    // if we're mallformed we might want to use xmlRecoverMemcory(char*, int)
    if (data->doc == NULL) {
        rb_raise( rb_eRuntimeError, "Failed to parse document" );
	    return Qnil;
    }
    
    dv = Data_Wrap_Struct( rb_cObject, fastxml_data_mark, fastxml_data_free, data );
    rb_iv_set(self, "@lxml_doc", dv );

    if (blk != Qnil)
        rb_yield( self );

    return self;
}


/* }}} fastxml_doc 
 */
