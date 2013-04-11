Installing the FMI++ Cython wrapper:

  * Install FMI++.
  * Edit all paths in "install.sh" and "setup.py" according to your system (Python, expat, FMI++).
  * Edit compiler flags in "setup.py" according to your system (MINGW or Linux).
  * Running "install.sh" in the console will produce "fmipp.pyd". Done!
  * When using "fmipp.pyd" in Python make sure the paths to all necessary libraries are included in environment variable PATH!!!
