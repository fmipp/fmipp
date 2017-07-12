# -------------------------------------------------------------------
# Copyright (c) 2013-2017, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

# Simplify an FMU's XML model description by removing all scalar variables whose causality is not defined as either 'input' or 'output'. Also remove type definitions that are not related to input or output types.
def simplifyModelDescription( xmlModelDescription ):
	import xml.etree.ElementTree as et

	# parse model description and retrieve root element
	root = et.fromstring( xmlModelDescription )
	
	# get XML node listing the model variables
	modelVariables = root.find( "ModelVariables" )
	
	# get list of all XML nodes representing scalar variables
	scalars = modelVariables.findall( "./ScalarVariable" )
	
	# set of declared types of scalar variables that are not removed
	declaredTypes = set()
	
	# iterate through all XML nodes representing scalar variables
	for elem in scalars:
		# check if node defines attribute 'causality'
		if 'causality' in elem.attrib:
			# get value of attribute causality
			causality = elem.attrib['causality'] 
			# remove node if causility is neither 'input' nor 'output'
			if not ( causality == 'input' or causality == 'output' ):
				modelVariables.remove( elem );
			else:
				# save type of scalar variable
				value = elem.findall( ".//*[@declaredType]" )
				if len( value ) > 1:
					print( "WARNING: scalar variable has more than one type defined" )
					declaredTypes.add( value[0].attrib['declaredType'] )
				else:
					# attribute 'causality' not defined --> delete XML node
					modelVariables.remove( elem );

	# get XML node listing the type definitions
	typeDefinitions = root.find( "TypeDefinitions" )
	
	# get list of all XML nodes representing types
	types = typeDefinitions.findall( "./Type" )
	
	# iterate through all XML nodes representing types
	for elem in types:
		# remove node if the type it declares is not used by remaining scalar variables
		if elem.attrib['name'] not in declaredTypes:
			typeDefinitions.remove( elem )
			
	# return simpified XML model description as string
	return et.tostring( root )


# Simplify an FMU by removing all scalar variables whose causality is not defined as either 'input' or 'output' from its XML model description. Also remove type definitions that are not related to input or output types. Attention: The changes applied to the FMU are permanent.
def simplifyFMU( fmuFileName ):
	import sys, os, zipfile, zlib

	# check if specified file is indeed a zip file
	if not zipfile.is_zipfile( fmuFileName ):
		print( '%s is not a valid ZIP archive' % fmuFileName )
		sys.exit()

	# access FMU
	fmuIn = zipfile.ZipFile( fmuFileName, 'r' )

	# output zip archive
	tmpFmuFileName = fmuFileName + '.tmp'
	fmuOut = zipfile.ZipFile( tmpFmuFileName, 'w', compression = zipfile.ZIP_DEFLATED )

	try:
		# access FMU model description		
		xmlModelDescription = fmuIn.read( 'modelDescription.xml' )

		# simplify model description
		simplifiedModelDesciption = simplifyModelDescription( xmlModelDescription )

		# iterate through content of FMU
		for item in fmuIn.infolist():
			# write all content to new archive, but use the simlified model description
			if not item.filename == 'modelDescription.xml':
				buffer = fmuIn.read( item.filename )
				fmuOut.writestr( item, buffer )
			else:
				fmuOut.writestr( 'modelDescription.xml', simplifiedModelDesciption )
	except KeyError:
		print( '\nInvalid FMU: no modelDescription.xml found in %s\n' % fmuFileName )

	# print info about simplified FMU
	import datetime
	for info in fmuOut.infolist():
		print( info.filename )
		print( '\tComment:\t', info.comment )
		print( '\tModified:\t', datetime.datetime(*info.date_time) )
		print( '\tSystem:\t\t', info.create_system, '(0 = Windows, 3 = Unix)' )
		print( '\tZIP version:\t', info.create_version )
		print( '\tCompressed:\t', info.compress_size, 'bytes' )
		print( '\tUncompressed:\t', info.file_size, 'bytes' )
		print()
	
	# close zip archives
	fmuIn.close()
	fmuOut.close()

	# replace original FMU with simplified FMU
	os.remove( fmuFileName );
	os.rename( tmpFmuFileName, fmuFileName )



if __name__ == '__main__':

	if len( sys.argv ) != 2:
		print( 'Usage:\n\tpython simplifyModelDescription.py <fmu-file-name>\n' )
		sys.exit()

	fmuFileName = sys.argv[1]
	
	simplifyFMU( fmuFileName )

