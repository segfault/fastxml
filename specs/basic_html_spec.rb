# $Id$
$: << '../ext'
$: << './ext'

require 'fastxml'

describe FastXml::Doc, " doing html parsing" do
  before(:all) do
    @data_raw = open( "./test_data/hasno_feed.html" )
    @data_ary = @data_raw.readlines
    @data_str = @data_ary.join('')
  end

  before do
    @data_raw.rewind if @data_raw
  end

  after(:all) do
    @data_raw.close if @data_raw
  end


  it 'should parse string input' do
    @data_str.should_not be_nil
    doc = FastXml::Doc.new( @data_str, {:html=>true} )
    doc.should_not be_nil
    doc.to_s.should_not be_nil
  end

  it 'should parse array input' do
    @data_ary.should_not be_nil
    doc = FastXml::Doc.new( @data_ary, {:html=>true} )
    doc.should_not be_nil
    doc.to_s.should_not be_nil
  end
  
  it 'should be able to parse large files' do
    raw_data = open( "./test_data/twitter_public.html" ).readlines.join('')
    doc = FastHtml( raw_data )
    doc.should_not be_nil
    doc.to_s.should_not be_nil
    doc.to_s.length.should >= 30000
    doc.root.should_not be_nil
    (doc/"").should_not be_nil
    doc.root.children.should_not be_nil
  end

  it 'should be able to handle the cnn site' do
    raw_data = open( "./test_data/cnn_main.html" ).readlines.join('')
    doc = FastHtml( raw_data )
    doc.should_not be_nil
    doc.to_s.should_not be_nil
    doc.to_s.length.should >= 10000
  end

end
