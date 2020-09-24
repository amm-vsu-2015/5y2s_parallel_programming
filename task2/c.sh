# $ sh c.sh
# prerequirements:
# - brew install libomp
# PS: Apple's clang compiler doesn't include openMP by default.

c++ -Xpreprocessor -fopenmp -lomp app.cpp -o app -std=c++11 -stdlib=libc++
./app

#clang -Xpreprocessor -fopenmp -lomp app.cpp -o app
#c++ app.cpp -o app -std=c++11 -stdlib=libc++
