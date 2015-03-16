def f(t)
  t
end

def looper(t, interval, source_end)
  ilo = ((t - source_end) / interval.to_f).floor + 1
  ilo = [ilo, 0].max
  ihi = (t / interval.to_f).floor
  sum = 0.0;
  (ilo..ihi).each do |i|
    sum += f(t - i * interval);
  end
  puts("t=#{t}, ilo=#{ilo}, ihi=#{ihi}, sum=#{sum}")
end

# (-8..16).each {|t| looper(t.to_f, 3, 8)}
