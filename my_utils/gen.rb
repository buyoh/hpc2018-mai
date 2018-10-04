
Dir.chdir(File.expand_path(File.dirname($0))) do
    sta = Time.now.strftime("%m%d_%H%M%S")
    system "..\\x64\\Release\\HPC2018.exe -j > ./tmp/replay#{sta}.json"
end
