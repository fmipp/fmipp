# -------------------------------------------------------------------
# Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

# Extract an FMU.
def extractFMU( fmuFilePath, outputDirPath ):

	import os, zipfile, urllib.parse, urllib.request

	# Check if specified file is indeed a zip file.
	if not zipfile.is_zipfile( fmuFilePath ):
		print( '%s is not a valid ZIP archive' % fmuFilePath )
		return

	# Check if output directory exists.
	if not os.path.isdir( outputDirPath ):
		print( '%s is not a valid path' % outputDirPath )
		return

	try:
		# Extract FMU file name from complete path.
		fmuFileName = os.path.split( fmuFilePath )[1];
		# Split FMU file name (name & extension).
		fmuSplitFileName = os.path.splitext( fmuFileName )
		
		# Check if this is a valid FMU file name.
		if '.fmu' != fmuSplitFileName[1]:
			print( '%s is not a valid FMU file name' % fmuFileName )

		# Extract model name from FMU file name.
		fmuModelName = fmuSplitFileName[0]

		# Access FMU.
		fmu = zipfile.ZipFile( fmuFilePath, 'r' )

		# Create sub-directory in output directory.
		extractDirPath = os.path.join( outputDirPath, fmuModelName )
		try:
			os.mkdir( extractDirPath )
		except OSError: # Directory already exists
			print( 'directory already exists: %s' % extractDirPath )

		# Extract FMU to output directory.
		fmu.extractall( extractDirPath )
		
		# Return URI to extracted FMU.
		return urllib.parse.urljoin( 'file:', urllib.request.pathname2url( extractDirPath ) ) 
	except:
		print( 'failed to extract file: %s' % fmuFilePath )


if __name__ == '__main__':

	import sys

	if len( sys.argv ) != 3:
		print( 'Usage:\n\tpython extractFMU.py <path-to-fmu> <path-to-output-dir>\n' )
		sys.exit()

	fmuFilePath = sys.argv[1]
	outputDirPath = sys.argv[2]

	extractDirURI = extractFMU( fmuFilePath, outputDirPath )
	print( 'extracted FMU to: %s' % extractDirURI )
