# RAPIDO Documentation
Assuming [Doxygen](https://www.doxygen.nl/manual/starting.html) is installed, follow these steps to produce the RAPDIO documentation:

1. Ensure you have a copy of the RAPIDO `master` branch checked out (only need read permissions)
```
git checkout https://github.com/jkguiang/rapido.git
```
2. Perform a separate `git checkout` for the `docs` branch (need write permissions)
```
git checkout -b docs git@github.com:jkguiang/rapido.git rapido-docs
```
3. Make the docs using the Doxygen configuration file (`doxyfile`) included here
```
doxygen doxyfile
```
4. Wait for changes to propogate to https://jkguiang.github.io/rapido/html/index.html
