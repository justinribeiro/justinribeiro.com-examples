<?php

$myGithubQuery	= "https://github.com/justinribeiro.atom";

$myCacheFile 		= "github.json"; 	// JDR: name the cache file whatever you like
$myCacheCycle 		= "600"; 		// JDR: in seconds

// JDR: technically we don't need this really, but if you ever need other header info, this is the way
$setContextOptions = array(
	'http'=> array(
    	'method'=>"GET"
	)
);
$createContext = stream_context_create($setContextOptions);

// JDR: let's get our cache file, returns false if file does not exist
$getCacheFileModTime = @filemtime( $myCacheFile );

if ( !$getCacheFileModTime || ( time() - $getCacheFileModTime >= $myCacheCycle) )
{
	// JDR: go get me some json, returns false if error
	$getGithubData = file_get_contents($myGithubQuery, false, $createContext);

	if ($getGithubData) {

		// write the JSON to file, return it to the caller
		$xmltoobj = simplexml_load_string($getGithubData);
		$objtojson = json_encode($xmltoobj);

		file_put_contents( $myCacheFile, $objtojson );
		$outputJSON = $objtojson;
	}
	else {
		// JDR: uh oh, we have a problem, write some simple error json to tell us on the frontend
		$errorArray = array('error' => 'There was a problem getting data from the Github RSS, please try again.');
		$outputJSON = json_encode($errorArray);
	}
}
else
{
	// JDR: get the cache file
	$outputJSON = file_get_contents($myCacheFile);
}

// JDR: gzip it if we can
ob_start('ob_gzhandler');
header('Content-type: application/json');
echo $outputJSON;

?>