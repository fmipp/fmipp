function createFMU( modelID, classFileName, extra, useJVM )
% CREATEFMU  Create an FMU from a class derived from 'fmipputils.FMUBase'.
%
%   CREATEFMU( MODEL_ID, CLASS_FILE_NAME, EXTRA )
%   CREATEFMU( MODEL_ID, CLASS_FILE_NAME, EXTRA, USE_JVM )
%
%   This function call creates an FMU using the model identifier 
%   specified in MODEL_ID. Input argument CLASS_FILE_NAME specifies 
%   the class definition file. Additional files, e.g., defining 
%   functions called by method doStep(...), can be specified using 
%   input argument EXTRA.
%
%   Boolean input parameter USE_JVM can be used to control whether 
%   MATLAB is started with our without the Java Virtual Machine. Set
%   it to 'true' in case the Java Virtual Machine should be started 
%   at MATLAB's startup or to 'false' if not (default). Note that
%   some of MATLAB's functions require the Java Virtual Machine to
%   run, e.g., xlsread(...)

    if nargin == 3
	   useJVM = false;
	end

	if ~ischar( modelID )
		error( 'input argument MODEL_ID is not a string' );
	end

	if ~ischar( classFileName )
		error( 'input argument CLASS_FILE_NAME is not a string' );
	end

	if ~ischar( classFileName )
		error( 'input argument CLASS_FILE_NAME is not a string' );
	end

	if ~islogical( useJVM )
		error( 'input argument USE_JVM is not a string' );
	end

	% Define helper function for writing cell arrays to file.
	function writeCellArrayToFile( cellArray, type, fileID )
		for i = cellArray
			fprintf( fileID, '%s\t:\t%s\n', char(type), char(i{1}) );
		end
	end

	% Parse class definition file name.
	[ pathstr, className, ext ] = fileparts( classFileName );

	% Instantiate object.
	obj = eval( className );

	% Check if instance does indeed derive from class 'fmipputils.FMUBase'.
	if ~isa( obj, 'fmipputils.FMUBase' )
		error( [ 'class ', className, ' is not derived "fmipputils.FMUBase"' ] );
	end

	% Run init() function. This initializes the object and defines all inputs, outputs, etc.
	obj.init();

	% Open file to save input variable names. This file will be used by script 'matlab_fmu_create.py'.
	inputsFileID = fopen( 'inputs.txt', 'w' );

	writeCellArrayToFile( obj.realInputNames_, 'Real', inputsFileID );
	writeCellArrayToFile( obj.integerInputNames_, 'Integer', inputsFileID );
	writeCellArrayToFile( obj.booleanInputNames_, 'Boolean', inputsFileID );
	writeCellArrayToFile( obj.stringInputNames_, 'String', inputsFileID );

	% Treat parameters like inputs (needs to be fixed).
	writeCellArrayToFile( obj.realParameterNames_, 'Real', inputsFileID );
	writeCellArrayToFile( obj.integerParameterNames_, 'Integer', inputsFileID );
	writeCellArrayToFile( obj.booleanParameterNames_, 'Boolean', inputsFileID );
	writeCellArrayToFile( obj.stringParameterNames_, 'String', inputsFileID );

	fclose( inputsFileID );

	% Open file to save output variable names. This file will be used by script 'matlab_fmu_create.py'.
	outputsFileID = fopen( 'outputs.txt', 'w' );

	writeCellArrayToFile( obj.realOutputNames_, 'Real', outputsFileID );
	writeCellArrayToFile( obj.integerOutputNames_, 'Integer', outputsFileID );
	writeCellArrayToFile( obj.booleanOutputNames_, 'Boolean', outputsFileID );
	writeCellArrayToFile( obj.stringOutputNames_, 'String', outputsFileID );

	fclose( outputsFileID );

	% Run script 'matlab_fmu_create.py'.
	pyScriptPath = fullfile( getenv( 'MATLAB_FMIPP_ROOT' ), 'matlab_fmu_create.py' );
	if true == useJVM
		cmd = [ 'python.exe ' pyScriptPath ' -v -J -m ' char(modelID) ' -c ' classFileName ' -i inputs.txt -o outputs.txt -I "' matlabroot '" ' char(extra) ];
		[status,cmdout] = system( cmd, '-echo' );
	else
		cmd = [ 'python.exe ' pyScriptPath ' -v -m ' char(modelID) ' -c ' classFileName ' -i inputs.txt -o outputs.txt -I "' matlabroot '" ' char(extra) ];
		[status,cmdout] = system( cmd, '-echo' );
	end
end