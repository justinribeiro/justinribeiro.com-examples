<html>
<head>
<title>SAM - WPM point to point transaction support testcase</title>

<?php

// test set up data...
//
$endpoints = 'localhost:7278:BootstrapBasicMessaging';
$bus = 'Bus1';
$target = 'InboundBasicMessaging';
$queuename = 'queue://testqueue';

?>
</head>

<body>
<h1>SAM - WPM Point to Point transaction support testcase</h1>
This testcase will exercise the SAM commit and rollback capabilities
using the WPM protocol to a WebSphere Platform Messaging server.
<p>
The tests will use the following set up:<br>
Endpoints:          <b><?php echo $endpoints ?></b><br>
Target chain:       <b><?php echo $target ?></b><br>
Bus:                <b><?php echo $bus ?></b><br>
Queue:              <b><?php echo $queuename ?></b><br>
<hr/>
Attempting connection with manual transaction control -
<?php
$conn = new SAMConnection();

if ($conn->connect(SAM_WPM, array(SAM_ENDPOINTS => $endpoints,
                   SAM_BUS => $bus, SAM_TARGETCHAIN => $target,
                   SAM_TRANSACTIONS => SAM_MANUAL))) {
    echo '<font color="green"><b>OK</b></font>';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Sending test message -
<?php
if ($conn) {
   $msg = new SAMMessage('This is a test message');
   $msg->header->SAM_TYPE = SAM_TEXT;

   $correlid = $conn->send($queuename, $msg);
   if (!$correlid) {
       echo '<font color="red"><b>FAILED</b></font>'." ($conn->errno) $conn->error";
   } else {
       echo '<font color="green"><b>OK</b></font> correlID='.$correlid;
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Attempting 2nd send -
<?php
if ($conn) {
   $msg = new SAMMessage('This is a 2nd test message');
   $correlid2 = $conn->send($queuename, $msg, array(SAM_SELECT => $correlid));
   if (!$correlid) {
       echo '<font color="red"><b>FAILED</b></font>'." ($conn->errno) $conn->error";
   } else {
       echo '<font color="green"><b>OK</b></font> correlID='.$correlid;
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Creating 2nd connection -
<?php
$conn2 = new SAMConnection();

if ($conn2->connect(SAM_WPM, array(SAM_ENDPOINTS => $endpoints, SAM_BUS => $bus, SAM_TARGETCHAIN => $target))) {
    echo '<font color="green"><b>OK</b></font>';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Attempting receive using 2nd connection -<br/>
<?php
if ($conn2) {
   $msg = $conn2->receive($queuename, array(SAM_WAIT => 5000));

   if ($msg) {
       echo '<font color="red"><b>FAILED</b></font> Oops, that message shouldn\'t have been accessible!';
   } else {
       if ($conn2->errno == SAM_RECEIVE_TIMEDOUT) {
           echo '<font color="green"><b>OK</b></font>'." ($conn2->errno) $conn2->error";
       } else {
           echo '<font color="red"><b>FAILED</b></font> XMS receive failure! '." ($conn2->errno) $conn2->error";
       }
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Committing the messages -<br/>
<?php
if ($conn) {
   if (!$conn->commit()) {
       echo '<font color="red"><b>FAILED</b></font> commit failed! '." ($conn->errno) $conn->error";
   } else {
       echo '<font color="green"><b>OK</b></font>';
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Attempting receive -<br/>
<?php
if ($conn2) {
   $msg = $conn2->receive($queuename);

   if ($msg) {
       foreach ($msg->header as $key => $value) {
           echo 'property <b>'.$key.'</b> set to <b>'.$value.'</b><br/>';
       }
   }

   if (!$msg) {
       echo '<font color="red"><b>FAILED</b></font>'." ($conn->errno) $conn->error";
   } else {
       echo '<font color="green"><b>OK</b></font> '."message=$msg->body";
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Attempting 2nd receive -<br/>
<?php
if ($conn2) {
   $msg = $conn2->receive($queuename);

   if ($msg) {
       foreach ($msg->header as $key => $value) {
           echo 'property <b>'.$key.'</b> set to <b>'.$value.'</b><br/>';
       }
   }

   if (!$msg) {
       echo '<font color="red"><b>FAILED</b></font>'." ($conn->errno) $conn->error";
   } else {
       echo '<font color="green"><b>OK</b></font> '."message=$msg->body";
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Sending test message -
<?php
if ($conn) {
   $msg = new SAMMessage('This is a 3rd test message');
   $msg->header->SAM_TYPE = SAM_TEXT;

   $correlid = $conn->send($queuename, $msg);
   if (!$correlid) {
       echo '<font color="red"><b>FAILED</b></font>'." ($conn->errno) $conn->error";
   } else {
       echo '<font color="green"><b>OK</b></font> correlID='.$correlid;
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Rolling back the message -<br/>
<?php
if ($conn) {
   if (!$conn->rollback()) {
       echo '<font color="red"><b>FAILED</b></font> rollback failed! '." ($conn->errno) $conn->error";
   } else {
       echo '<font color="green"><b>OK</b></font>';
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Attempting receive using 2nd connection -<br/>
<?php
if ($conn2) {
   $msg = $conn2->receive($queuename, array(SAM_WAIT => 5000));

   if ($msg) {
       echo '<font color="red"><b>FAILED</b></font> Oops, that message shouldn\'t have been accessible!';
   } else {
       echo '<font color="green"><b>OK</b></font>'." ($conn2->errno) $conn2->error";
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Check connection still shows as connected -
<?php
if (!$conn->isConnected()) {
    echo '<font color="red"><b>FAILED</b></font>';
} else {
    echo '<font color="green"><b>OK</b></font>   ';
}
?>
<p/>
<hr/>
Attempting disconnect with connected connection -
<?php
if (!$conn->disconnect()) {
    echo '<font color="red"><b>FAILED</b></font>';
} else {
    echo '<font color="green"><b>OK</b></font>   ';
}
?>
<p/>
<hr/>
</body>
</html>