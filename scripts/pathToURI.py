
# Convert path to URI.
def pathToURI( path ):
	import urlparse, urllib
	return urlparse.urljoin( 'file:', urllib.pathname2url( path ) )


# Concatenate two paths and return URI of resulting path
def concatPathsToURI( path1, path2 ):
	import urlparse, urllib, os.path
	return urlparse.urljoin( 'file:', urllib.pathname2url( os.path.join( path1, path2 ) ) )