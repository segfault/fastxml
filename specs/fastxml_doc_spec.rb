# $Id$
$: << '../ext'
$: << './ext'

require 'fastxml'

describe FastXml::Doc, " functionality" do
  before(:all) do
    data_raw = open( "./test_data/hasno_feed.xml" )
    @data_ary = data_raw.readlines
    data_raw.close
    @data_str = @data_ary.join('')
    @doc = FastXml::Doc.new( @data_str )
  end

  it 'should have a root node accessor' do
    @doc.root.should_not be_nil
  end

  #it 'should be able to run namespaced xpath searches' do
  #  @doc.search( "/Atom:feed/Atom:entry").length.should > 1
  #end
  
  it 'should be able to run default namespace xpath searches' do
    @doc.search( '/feed/entry' ).length.should > 1
  end

end
