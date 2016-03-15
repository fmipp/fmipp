
# Convert path to URI.
def pathToURI( path ):
	import urllib.parse, urllib.request
	return urllib.parse.urljoin( 'file:', urllib.request.pathname2url( path ) )


# Concatenate two paths and return URI of resulting path
def concatPathsToURI( path1, path2 ):
	import urllib.parse, urllib.request, os.path
	return urllib.parse.urljoin( 'file:', urllib.request.pathname2url( os.path.join( path1, path2 ) ) )