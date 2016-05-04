function createFMU( scriptName, modelID, extra )

	function writeCellArrayToFile( cellArray, type, fileID )
		for i = cellArray
			%fprintf( '%s\t:\t%s\n', char(type), char(i{1}) );
			fprintf( fileID, '%s\t:\t%s\n', char(type), char(i{1}) );
		end
	end

	%run( scriptName );
	
	inputsFileID = fopen( 'inputs.txt', 'w' );
	
	global fmippexRealInputNames;
	writeCellArrayToFile( fmippexRealInputNames, 'Real', inputsFileID );

	global fmippexIntegerInputNames;
	writeCellArrayToFile( fmippexIntegerInputNames, 'Integer', inputsFileID );

	global fmippexBooleanInputNames;
	writeCellArrayToFile( fmippexBooleanInputNames, 'Boolean', inputsFileID );

	global fmippexStringInputNames;
	writeCellArrayToFile( fmippexStringInputNames, 'String', inputsFileID );

	fclose( inputsFileID );
	

	outputsFileID = fopen( 'outputs.txt', 'w' );

	global fmippexRealOutputNames;
	writeCellArrayToFile( fmippexRealOutputNames, 'Real', outputsFileID );

	global fmippexIntegerOutputNames;
	writeCellArrayToFile( fmippexIntegerOutputNames, 'Integer', outputsFileID );

	global fmippexBooleanOutputNames;
	writeCellArrayToFile( fmippexBooleanOutputNames, 'Boolean', outputsFileID );

	global fmippexStringOutputNames;
	writeCellArrayToFile( fmippexStringOutputNames, 'String', outputsFileID );
	
	fclose( outputsFileID );
	
	pyScriptPath = fullfile( getenv( 'MATLAB_FMIPP_ROOT' ), 'matlab_fmu_create.py' );
	cmd = [ 'python.exe ' pyScriptPath ' -v -m ' char(modelID) ' -s ' scriptName ' -i inputs.txt -o outputs.txt -I ' matlabroot ' ' char(extra) ];
	[status,cmdout] = system( cmd, '-echo' );
end