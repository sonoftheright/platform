# cd ./run_tree; ./_obscura --edit &
echo '################';
echo 'Make sure to build without the address sanitizer first!';
echo '################';

cd ./run
# cd ../
rm -rf ./example.trace
# instruments -l 10000 -t Time\ Profiler -D ./_obscura.trace -p $(pgrep _obscura)
# xcrun xctrace record --output ./_obscura_leaks.trace --template 'Leaks' --time-limit 10000ms --attach $(pgrep _obscura)
xcrun xctrace record --output ./example.trace --template 'Leaks' --time-limit 60s --launch -- ./example --ingame
kill $(pgrep example)
open ./example.trace
