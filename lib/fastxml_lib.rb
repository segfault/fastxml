# $Id$
module FastXml
  VERSION = "0.1.91" 
end

module FastXml::Common
  def children_of_type(type)
    self.search( "//#{type}" )
  end
  
  def each_child(&blk)
    self.children.each { |chld| yield chld }
  end
  
  def /(xpath)
    self.search( "/#{xpath.to_s}" )
  end
  
  def at(xpath)
    nodes = self.search( xpath )
    return nil unless nodes && nodes.length > 0
    nodes[0]
  end
  
  alias :to_s :display
end

class FastXml::Doc
  include FastXml::Common
  
  def doc?
    true
  end
  
  def doctype?
    nil
  end
  
  def forgiving?
    (@forgiving ||= false)
  end
  
  def  validate?
    (@validate_dtd ||= false)
  end
  
  def xpath
    "/"
  end
end

class FastXml::Node
  include FastXml::Common
  def doc?
    false
  end
end

class FastXml::NodeList
  def [](idx)
    self.entry(idx)
  end
  
  def first
    self.entry(0)
  end
  
  def last
    self.entry(-1)
  end
  
  def empty?
    return (length == 0)
  end
  
  def at(tgt)
    return self.entry( tgt.to_i ) if tgt =~ /^\d+$/
    ret = []
    each { |nd| ret << (nd/tgt).to_ary }
    ret.flatten!
  end
end


def FastXml(data=nil, opts = {}, &blk)
  FastXml::Doc.new( data, opts, &blk )
end

def FastHtml(data=nil, opts = {}, &blk)
  opts ||= {}
  opts[:html] = true
  FastXml::Doc.new( data, opts, &blk )
end
