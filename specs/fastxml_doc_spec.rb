# $Id$
$: << '../ext'
$: << './ext'

require 'fastxml'

describe FastXml::Doc, " functionality" do
  before(:all) do
    @filename = "./test_data/hasno_feed.xml"
    data_raw = open( @filename )
    @data_ary = data_raw.readlines
    data_raw.close
    @data_str = @data_ary.join('')
    @doc = FastXml::Doc.new( @data_str )
  end

  it 'should have a root node accessor' do
    @doc.should respond_to( :root )
    @doc.root.should_not be_nil
  end
  
  it 'should have a children accessor' do
    @doc.should respond_to( :children )
    @doc.children.should_not be_nil
  end
  
  it 'should be able to run default namespace xpath searches' do
    @doc.should respond_to( :search )
    @doc.search( '/feed/entry' ).length.should > 1
  end
  
  it 'should allow hpricot-style searching using the / operator' do
    @doc.should respond_to( '/' )
    (@doc/"feed/entry").length.should > 1
    (@doc/"feed/entry").length.should == @doc.search( '/feed/entry' ).length
  end

  it 'should provide a to_s method' do
    @doc.should respond_to( :to_s )
    @doc.to_s.should_not be_nil
  end

  it 'should provide an inspect method' do
    @doc.should respond_to( :inspect )
    @doc.inspect.should_not be_nil
  end
  
  it 'should provide a children_of_type method' do
    @doc.should respond_to( :children_of_type )
  end  
  
  it 'should provide an at method' do
    @doc.should respond_to( :at )
    @doc.at( "/feed" ).should_not be_nil
  end
  
  it 'should be able to parse objects with a readlines method' do
    rlm = FastXml( open( @filename ) )
    rlm.should_not be_nil
    rlm.to_s.should == @doc.to_s
    rlm.should_not == @doc
  end
end
