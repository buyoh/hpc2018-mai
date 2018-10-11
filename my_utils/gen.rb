
seed = gets.chomp.split*4

Dir.chdir(File.expand_path(File.dirname($0))) do
    sta = Time.now.strftime("%m%d_%H%M%S")
    system "..\\x64\\Release\\HPC2018.exe -j > ./tmp/replay#{sta}.json" if seed.empty?
    system "..\\x64\\Release\\HPC2018.exe -j -r #{seed[0]} #{seed[1]} #{seed[2]} #{seed[3]} > ./tmp/replay#{sta}-#{seed[0..3]*"-"}.json" unless seed.empty?
end
