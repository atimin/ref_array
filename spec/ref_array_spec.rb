require 'ref_array'

describe RefArray  do
  before(:all) do
    @ra = RefArray.new([1,2,3])
  end

  it 'should have refs to all arrays' do
    RefArray.refs.should  eql([[1,2,3]])
  end

  it 'should clear ref if linked object has deleted' do
    1.times do
      r = RefArray.new([3,4,5])
      RefArray.refs.should eql([[1,2,3], [3,4,5]])
    end
    GC.start
    RefArray.refs.should eql([[1,2,3], nil])
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
    @ra.to_a.should eql([1,2,-1])
    GC.start
    @ra.to_a.should eql([1,2,-1])
  end
end
