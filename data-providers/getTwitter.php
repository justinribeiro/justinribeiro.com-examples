<?php

// via http://davidwalsh.name/linkify-twitter-feed (2009!)
// don't need the hit on the front
function linkify_twitter_status($status_text) {
	// linkify URLs
	$status_text = preg_replace(
			'/(https?:\/\/\S+)/',
			'<a href="\1">\1</a>',
			$status_text
	);

	// linkify twitter users
	$status_text = preg_replace(
			'/(^|\s)@(\w+)/',
			'\1<a href="http://twitter.com/\2">@\2</a>',
			$status_text
	);

	// linkify tags
	$status_text = preg_replace(
			'/(^|\s)#(\w+)/',
			'\1<a href="http://search.twitter.com/search?q=%23\2">#\2</a>',
			$status_text
	);

	return $status_text;
}

$myTwitterQuery	= "https://twitter.com/statuses/user_timeline/justinribeiro.json?count=1";

$myCacheFile 		= "twitter.json"; 	// JDR: name the cache file whatever you like
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
	$getTwitterData = file_get_contents($myTwitterQuery, false, $createContext);

	if ($getTwitterData) {

		$status = json_decode($getTwitterData);

		$data = array(
			'quote' => linkify_twitter_status($status[0]->text),
			'link'	=> "http://twitter.com/justinribeiro/statuses/" . $status[0]->id_str,
			'date'	=> date("l, jS \of F", strtotime($status[0]->created_at))
		);

		// write the JSON to file, return it to the caller
		$objtojson = json_encode($data);

		file_put_contents( $myCacheFile, $objtojson );
		$outputJSON = $objtojson;
	}
	else {
		// JDR: uh oh, we have a problem, write some simple error json to tell us on the frontend
		$errorArray = array('error' => 'There was a problem getting data from the Twitter RSS, please try again.');
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