class T
  def []=(a,b)
    puts "#{self}[#{a}]=#{b}"
  end

  def [](a)
    puts "#{self}[#{a},#{c}]"
    self
  end

  def +(b)
    puts "#{self} + #{b}"
  end

  def b
    puts "#{self}.b"
    "123"
  end

  def a=(b)
    puts "#{self}.a=#{b}"
  end

  def a
    puts "#{self}.a"
    "123"
  end
end

T.new[T.new]+=T.new.b
T.new.a*=5

def test(a,b,c,d=1,e="wddwadwadwa", *argse,r,t,y,q:1,w:,u:1+2)
end
