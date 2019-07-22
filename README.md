## flow framework adopted to STAR FemtoDst data

Installation: 

1. install FemtoDst or use the existing installation
2. clone flow repository 
```
  git clone -b star https://github.com/eugene274/flow.git 
```
2. create build directory inside the flow source folder:
```
  cd flow 
  mkdir build
  cd build
```
3. build the project:
```
  cmake -DFEMTODST_HOME=/path/to/FemtoDst/package ..
  make
```
4. run the example chain from the build directory:
```
../macro/run.sh /path/to/input/file/or/filelist.list
```