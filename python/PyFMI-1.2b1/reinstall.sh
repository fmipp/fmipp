rm -rf /c/Tools/Python2.7.3/Lib/site-packages/pyfmi
rm -rf /c/Tools/Python2.7.3/Lib/site-packages/PyFMI-1.2b1-py2.7.egg-info
rm -rf build
/c/Tools/Python2.7.3/python.exe setup.py install --fmil-home=/c/Development/FMILibrary/install/ --fmipp-home=/c/Development/fmipp/
