function booleanInputValues = getBooleanInputValues( backend, booleanInputs, booleanInputSize )
   % Read current inputs.
   status = backend.getBooleanInputs( booleanInputs, booleanInputSize );
   if status ~= fmippex.fmiOK(); error( 'getBooleanInputs not successful' ); end
   
   booleanInputValues = NaN( 1, booleanInputSize );
   for i = 1 : booleanInputSize
      booleanInputValues(i) = fmippex.char_array_getitem( booleanInputs, i-1 );
   end
end