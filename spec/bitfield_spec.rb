require_relative 'spec_helper'

describe BitField do
  it "can be initialized with a size" do
    obj = BitField.new 10
    obj.should be_a(BitField)
  end

  it "has a to_s method" do
    BitField.new(10).should respond_to(:to_s)
  end

  it "has a clone method" do
    BitField.new(10).should respond_to(:clone)
  end

  it "has a size method" do
    BitField.new(10).should respond_to(:size)
  end

  it "has a flip method" do
    BitField.new(10).should respond_to(:flip)
  end

  it "has a count method" do
    BitField.new(10).should respond_to(:count)
  end

  it "has an each method" do
    BitField.new(10).should respond_to(:each)
  end

  it "has an each_index method" do
    BitField.new(10).should respond_to(:each_index)
  end

  it "has a [] method" do
    BitField.new(10).should respond_to(:[])
  end

  it "has a []= method" do
    BitField.new(10).should respond_to(:[]=)
  end

  it "has an initialize_clone method" do
    BitField.new(10).should respond_to(:initialize_clone)
  end

  it "is an Enumerable" do
    BitField.include? Enumerable
  end
end

describe BitField, "#[]" do
  it "returns an array for a range" do
    b = BitField.new(10)
    b[0..4].should == [0,0,0,0,0]
  end

  it "return a value for a position" do
    b = BitField.new(10)
    b[0].should == 0
  end

  it "returns nil if the position is out of bounds" do
    b = BitField.new(10)
    b[-1].should be_nil
  end
end

describe BitField, "#[]=" do
  it "sets a value n for position x" do
    b = BitField.new(10)
    b[5] = 1
  end

  it "assigns even values as 0" do
    b = BitField.new(10)
    b[5] = 2**10
    b[5].should == 0
  end

  it "assigns uneven values as 1" do
    b = BitField.new(10)
    b[9] = 2**10+1
    b[9].should == 1
  end

  it "sets a range of values if an array is assigned" do
    b = BitField.new(10)
    b[0..4] = Array.new(5) { 1 }
    b[0..4].should == Array.new(5) { 1 }
  end
end

describe BitField, "#size" do
  it "returns the size of the bitfield" do
    size = 10
    obj = BitField.new size
    obj.size.should == size
  end
end

describe BitField, "#to_s" do
  it "returns the bitfield as string" do
    b = BitField.new 10
    b[0] = 1
    b[2] = 1
    b[9] = 1
    b.to_s.should eq("1000000101")
  end
end

describe BitField, "#flip" do
  it "flips all bits in the bitfield" do
    obj = BitField.new 10
    obj.flip
    obj.to_s.should eq("1111111111")
  end

  it "flips bit n in the bitfield" do
    obj = BitField.new 10
    obj.flip 5
    obj[5].should == 1
  end
end

describe BitField, "#count" do
  it "counts all 1s in the bitfield" do
    obj = BitField.new 10
    5.times do |i|
      obj[i] = 1
    end
    obj.count.should == 5
  end
end

describe BitField, "#each" do
  it "returns an enumerator if no block is given" do
    obj = BitField.new 10
    obj.each.is_a? Enumerator
  end
end

describe BitField, "#each_index" do
  it "returns an enumerator if no block is given" do
    obj = BitField.new 10
    obj.each_index.is_a? Enumerator
  end
end
