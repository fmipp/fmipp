function stringInputValues = getStringInputValues( backend, stringInputs, stringInputSize )
   % Read current inputs.
   status = backend.getStringInputs( stringInputs, stringInputSize );
   if status ~= fmippex.fmiOK(); error( 'getStringInputs not successful' ); end
   
   stringInputValues = {}
   for i = 1 : stringInputSize
      stringInputValues(i) = cellstr( fmippex.string_array_getitem( stringInputs, i-1 ) );
   end
end