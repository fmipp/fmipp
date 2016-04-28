function integerInputValues = getIntegerInputValues( backend, integerInputs, integerInputSize )
   % Read current inputs.
   status = backend.getIntegerInputs( integerInputs, integerInputSize );
   if status ~= fmippex.fmiOK(); error( 'getIntegerInputs not successful' ); end
   
   integerInputValues = NaN( 1, integerInputSize );
   for i = 1 : integerInputSize
      integerInputValues(i) = fmippex.int_array_getitem( integerInputs, i-1 );
   end
end