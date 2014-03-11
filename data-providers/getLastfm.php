<?php

$myLastfmAPIkey = "";
$myLastfmUser	= "justinribeiro";
$myLastfmQuery	= "http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&user=" . $myLastfmUser . "&api_key=" . $myLastfmAPIkey . "&format=json";

$myCacheFile 		= "lastfm.json"; 	// JDR: name the cache file whatever you like
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
	$getLastfmData = file_get_contents($myLastfmQuery, false, $createContext);

	if ($getLastfmData) {

		// write the JSON to file, return it to the caller
		file_put_contents( $myCacheFile, $getLastfmData );
		$outputJSON = $getLastfmData;
	}
	else {
		// JDR: uh oh, we have a problem, write some simple error json to tell us on the frontend
		$errorArray = array('error' => 'There was a problem getting data from the Lastfm JSON, please try again.');
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