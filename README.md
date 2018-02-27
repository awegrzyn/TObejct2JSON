# TObject2Json

### Install and load dependencies
~~~
aliBuild --default o2-daq build QualityControl
alienv enter QualityControl/latest-o2-daq
~~~

### Compilation
~~~
mkdir build && cd build
cmake ..
make -j
~~~

### Run
~~~
./build/bin/tobject2json
~~~


### Documentation
* ZeroMQ http://api.zeromq.org/4-2:_start
