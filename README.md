## flow framework adopted to HADES DataTree data

Installation: 

1. install HADES DataTree or use the existing installation
2. clone flow repository 
```
  git clone https://github.com/mam-mih-val/DT_Flow
```
2. create build directory inside the flow source folder:
```
  cd DT_Flow 
  mkdir build
  cd build
```
3. build the project:
```
  cmake -DataTree_HOME=/path/to/FemtoDst/package ..
  make
```
