/*
 * Document-class: FastXml::NodeList
 *
 * call-seq:
 *  doc = FastXml( docfile ) # from FastXml::Doc
 *  list = doc.root.children # pull the FastXml::NodeList comprised of the Element's children
 *  puts "The document root element has %d children." % list.length
 *  list.each { |e| puts e.name } # iterate over each element and print it's name
 *  puts list[3]                  # print the 4th element in the list 
 */
// Please see the LICENSE file for copyright, licensing and distribution information

#include "fastxml.h"
#include "fastxml_node.h"
#include "fastxml_doc.h"
#include "fastxml_nodelist.h"

/* {{{ fastxml_nodelist
 */
void Init_fastxml_nodelist()
{
	#ifdef RDOC_SHOULD_BE_SMARTER__THIS_IS_NEVER_RUN
    rb_mFastXml = rb_define_module( "FastXml" );
	#endif
    rb_cFastXmlNodeList = rb_define_class_under( rb_mFastXml, "NodeList", rb_cObject );
	
    rb_include_module( rb_cFastXmlNodeList, rb_mEnumerable );	
    rb_define_method( rb_cFastXmlNodeList, "initialize", fastxml_nodelist_initialize, 0 );
    rb_define_method( rb_cFastXmlNodeList, "length", fastxml_nodelist_length, 0 );
    rb_define_method( rb_cFastXmlNodeList, "each", fastxml_nodelist_each, 0 );
    rb_define_method( rb_cFastXmlNodeList, "entry", fastxml_nodelist_entry, 1 );
	rb_define_method( rb_cFastXmlNodeList, "to_ary", fastxml_nodelist_entry, 0 );
}

VALUE fastxml_nodelist_inspect(VALUE self)
{
    VALUE dv;
    VALUE *argv;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );

    argv = ALLOCA_N( VALUE, 4 );
    argv[0] = rb_str_new2( "#<%s:0x%x %d>" );
    argv[1] = CLASS_OF( self );
    argv[2] = rb_obj_id( self );
    argv[3] = fastxml_nodelist_length( self );
    return rb_f_sprintf( 4, argv );
}

VALUE fastxml_nodelist_initialize(VALUE self)
{
    return self;
}

/* Return the length of the FastXml::NodeList
 *
 * call-seq:
 *   puts doc.children.length
 */
VALUE fastxml_nodelist_length(VALUE self)
{
    VALUE dv;
    xmlNodePtr cur;
    fxml_data_t *data;

    dv = rb_iv_get( self, "@lxml_doc" );    
    Data_Get_Struct( dv, fxml_data_t, data );
	
	if (data->list_len == EMPTY_NODELIST) {              
		data->list_len = 0;
		
        cur = data->list;
		while (cur != NULL) {
			data->list_len++;
			cur = cur->next;
		}
	}
	
	return rb_int2inum( data->list_len );
}

VALUE fastxml_nodelist_obj_to_ary(fxml_data_t *root)
{
	VALUE ret;
    xmlNodePtr cur = root->list;	
	
 	ret = rb_ary_new();
    while (cur != NULL) {        
        rb_ary_push( ret, fastxml_raw_node_to_obj( cur ) );
		cur = cur->next;
	}	
	
	return ret;
}

VALUE fastxml_nodeset_obj_to_ary(fxml_data_t *root)
{
	VALUE ret;
	xmlNodePtr cur, sub = NULL;
	int i;

 	ret = rb_ary_new();
    if (root->xpath_obj->nodesetval->nodeTab != NULL) {
        cur = *root->xpath_obj->nodesetval->nodeTab;	
        for (i = 0; i < root->list_len; i++) {
            if (cur->type != XML_ELEMENT_NODE)
                continue;
            
            rb_ary_push( ret, fastxml_raw_node_to_obj( cur ) );
            sub = cur->next;
            while (sub != NULL) {
                rb_ary_push( ret, fastxml_raw_node_to_obj( sub ) );
                sub = sub->next;
            }
		    cur++;
        }
    }
    
	
	return ret;
}

VALUE fastxml_nodelist_gen_list(VALUE self, fxml_data_t *data)
{
	VALUE lst = rb_iv_get( self, "@list" );

	if (lst == Qnil) {
        if (data->xpath_obj != NULL) {
			lst = fastxml_nodeset_obj_to_ary( data );
			rb_iv_set( self, "@list", lst );
		} else {
			lst = fastxml_nodelist_obj_to_ary( data );
			rb_iv_set( self, "@list", lst );
		}
	}

	return lst;
}

VALUE fastxml_nodelist_to_ary(VALUE self)
{
	VALUE dv;
	fxml_data_t *data;
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );	
	return fastxml_nodelist_gen_list( self, data );		
}

/* Iterate over the list yielding each FastXml::Node contained
 *
 * call-seq:
 *   puts doc.children.each { |e| puts e.name }
 */
VALUE fastxml_nodelist_each(VALUE self)
{
	VALUE lst, dv;
	fxml_data_t *data;
	int i;
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );	
	lst = fastxml_nodelist_gen_list( self, data );
	
	for (i=0; i<RARRAY(lst)->len; i++) {
		rb_yield( RARRAY(lst)->ptr[i] );
    }
	
	return self;
}

/* Returns the element (FastXml::Node) from the FastXml::NodeList at idx 
 * position. 0-based indexing. Ranges are not supported as of yet 
 * (Patches welcome).
 *
 * call-seq:
 *   puts doc.children[0]  # 
 *   puts doc.children[-1] # last element in the list
 */
VALUE fastxml_nodelist_entry(VALUE self, long idx)
{
	VALUE lst, dv;
	fxml_data_t *data;
	
	dv = rb_iv_get( self, "@lxml_doc" );
	Data_Get_Struct( dv, fxml_data_t, data );	
	lst = fastxml_nodelist_gen_list( self, data );
	if (idx > 0) // this comes in offset by 1
		idx = idx-1;
	// TODO: find out why this is provided offset by 1 and not 0-based

	return rb_ary_entry( lst, idx );
}

/* }}} fastxml_nodelist
 */
