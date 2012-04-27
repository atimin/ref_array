require 'ref_array'

describe RefArray  do
  before(:all) do
    @ra = RefArray.new([1,2,3])
  end

  it 'should have to_a method' do
    @ra.to_a.should eql([1,2,3])
  end

  it 'should use refernce' do
    1.times do
      r = @ra[1..2]
      r.to_a.should eql([2,3])
      r[1] = -1
    end
    GC.start
    @ra.to_a.should eql([1,2,-1])
  end
end
