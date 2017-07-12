% -------------------------------------------------------------------
% Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
% All rights reserved. See file FMIPP_LICENSE for details.
% -------------------------------------------------------------------

function licenseInfo()
   info1 = 'The FMI++ MATLAB Toolbox for Windows is based on code from the FMI++ Library and BOOST.';
   info2 = 'Also, it includes compiled libraries implementing the SUNDIALS CVODE integrator.';
   info3 = 'For detailed information on the respective licenses please refer to the license files provided here:';
   disp( info1 );
   disp( info2 );
   disp( info3 );
   disp( [ getenv( 'MATLAB_FMIPP_ROOT' ) '\license' ] );
end