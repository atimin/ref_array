##Experimental implementation array with slicing by reference

  ```ruby
  a = RefArray.new([1,2,3])
  b = a[0..1]
  b[1] = 999
  a.to #=> [1, 999, 3]
  ```

**It's experimental work don't considirate it as serious project!**



