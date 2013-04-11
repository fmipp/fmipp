Installing PyFMI using FMI++ integration functionalities:

  * Install FMI++.
  * Install PyFMI-1.2b1 from source (located at "PYFMI_HOME").
  * Copy "fmi.pyx" to "PYFMI_ROOT/src/pyfmi".
  * Copy "setup.py" and "reinstall.sh" to "PYFMI_HOME".
  * Adapt paths in "reinstall.sh" to fit your Python, FMIL and FMI++ installations.
  * Edit compiler flags in "setup.py" according to your system (MINGW or Linux).
  * Run "reinstall.sh" in console.
