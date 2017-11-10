# Monitoring

### Installation (CC7)
~~~
aliBuild --defaults root6 build zeromq root
~~~

aliBuild installation procedure: http://alisw.github.io/alibuild/o2-daq-tutorial.html

### Configuration
~~~
alienv load ROOT/latest-root6
alienv enter ZeroMQ/latest-root6
~~~

### Compilation
~~~
cd TObejct2JSON
mkdir build; cd build
cmake ...
make -j
~~~
