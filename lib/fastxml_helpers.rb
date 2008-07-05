# Easy to remember common aliases ala Hpricot...
#
# Please see the LICENSE file for copyright, licensing and distribution information

# Alias for FastXml::Doc.new
def FastXml(data=nil, opts = {}, &blk)
  FastXml::Doc.new( data, opts, &blk )
end

# Alias for FastXml::Doc.new(data, {:html => true}, blk)
def FastHtml(data=nil, opts = {}, &blk)
  opts ||= {}
  opts[:html] = true
  FastXml::Doc.new( data, opts, &blk )
end