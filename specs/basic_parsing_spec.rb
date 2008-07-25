# encoding: utf-8
$: << '../ext'
$: << './ext'

require 'fastxml'

describe FastXml::Doc, " when created" do
  before(:all) do
    @data_raw = open( "./test_data/hasno_feed.xml" )
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
    doc = FastXml::Doc.new( @data_str )
    doc.should_not be_nil
    doc.to_s.should_not be_nil
  end

  it 'should parse array input' do
    @data_ary.should_not be_nil
    doc = FastXml::Doc.new( @data_ary )
    doc.should_not be_nil
    doc.to_s.should_not be_nil
  end
  
  it 'should be able to parse large files' do
    raw_data = open( "./test_data/xslspec.xml" ).readlines.join('')
    doc = FastXml( raw_data )
    doc.should_not be_nil
    doc.to_s.should_not be_nil
    doc.to_s.length.should >= 1584496
    doc.root.should_not be_nil
    (doc/"").should_not be_nil
    doc.root.children.should_not be_nil
  end

  it 'should be able to process io from open' do
    doc = FastXml( open( "./test_data/hasno_feed.xml" ) )
    doc.should_not be_nil
    doc.to_s.should_not be_nil
    doc.to_s.length.should >= 200
  end

end
