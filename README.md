# RAPIDO Documentation
Uses JS/CSS from [Doxygen Awesome](https://github.com/jothepro/doxygen-awesome-css).
Assuming [Doxygen](https://www.doxygen.nl/manual/starting.html) is installed, follow these steps to produce the RAPDIO documentation:

1. Ensure you have a copy of the RAPIDO cloned (only need read permissions)
```
git clone https://github.com/jkguiang/rapido.git
```
2. Create a separate clone (be sure to use `--recursive`) of the `docs` branch of the RAPIDO repo (need write permissions)
```
git clone --recursive -b docs git@github.com:jkguiang/rapido.git rapido-docs
```
3. Make the docs using the Doxygen configuration file (`doxyfile`) included here
```
doxygen doxyfile
```
4. Push the changes
```
git add html
git push origin docs
```
5. Wait for changes to propogate to https://jkguiang.github.io/rapido/html/index.html
