<html>
<head>
<title>SAM - MQTT pub/sub Testcase</title>

<?php
if ( ! extension_loaded('sam') ) {
    dl('php_sam.dll');
}

// test set up data...
//
$queuename = 'topic://test/x/y';
$protocol = SAM_MQTT;

?>
</head>

<body>
<h1>SAM - MQTT pub/sub Testcase</h1>
This testcase will exercise the SAM connect, subscribe, receive, unsubscribe and disconnect capabilities
using the MQTT protocol to an MQSeries Messaging server or MicroBroker.
<p>
The tests will use the following set up:<br>
Protocol:           <b><?php echo $protocol ?></b><br>
Queue:              <b><?php echo $queuename ?></b><br>
<hr/>
Attempting connection with insufficient parameters -<br>
<?php
$conn = new SAMConnection();
if (!$conn->connect()) {
    echo '<font color="green"><b>OK</b></font>   ';
} else {
    echo '<font color="red"><b>FAILED</b></font> $conn='.$conn;
}
?>
<p/>
<hr/>
Attempting connection to invalid host -<br>
<?php
$conn = new SAMConnection();
if (!$conn->connect($protocol, array(SAM_HOST => 'non-existent_host'))) {
    echo '<font color="green"><b>OK</b></font>   ';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Checking unconnected connection shows as unconnected -<br>
<?php
if (!$conn->isConnected()) {
    echo '<font color="green"><b>OK</b></font>   ';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Attempting disconnect with unconnected connection -<br>
<?php
if (!$conn->disconnect()) {
    echo '<font color="green"><b>OK</b></font>   ';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Attempting connection to valid host -<br>
<?php
$conn1 = new SAMConnection();

if ($conn1->connect($protocol)) {
    echo '<font color="green"><b>OK</b></font>';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Attempting a second connection to valid host -<br>
<?php
$conn = new SAMConnection();
if ($conn->connect($protocol, array(SAM_MQTT_CLEANSTART => false))) {
    echo '<font color="green"><b>OK</b></font>';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Attempting a receive with an invalid subscription id -<br>
<?php
if ($conn) {
   $msg = $conn->receive('invalid sub name as it is too long', array(SAM_WAIT => 5000));

   if (!$msg) {
       echo '<font color="green"><b>OK</b></font> receive failed with errno='.$conn->errno.' '.$conn->error.'<br>';
   } else {
       echo '<font color="red"><b>FAILED</b></font> Should not have received a message!';
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Attempting subscribe -<br>
<?php
if ($conn) {
   $sub = $conn->subscribe($queuename);

   if ($sub) {
       echo '<font color="green"><b>OK</b></font> subscribe suceeded sub id='.$sub.'<br>';
   } else {
       echo '<font color="red"><b>FAILED</b></font> Subscribe failed  with errno='.$conn->errno.' '.$conn->error.'<br>';
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Attempting receive via subscription -<br>
<?php
if ($conn) {
   $msg = $conn->receive($sub, array(SAM_WAIT => 5000));

   if (!$msg) {
       echo '<font color="green"><b>OK</b></font> receive failed with errno='.$conn->errno.' '.$conn->error.'<br>';
   } else {
       echo '<font color="red"><b>FAILED</b></font> Should not have received a message!';
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Attempting send to topic -<br>
<?php
if ($conn1) {
   $msg = new SAMMessage('This is a test message on topic test/x/y');
   $msg->header->SAM_TYPE = SAM_TEXT;

   $correlid = $conn1->send($queuename, $msg);
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
Attempting receive via subscription again -<br>
<?php
if ($conn) {
   $msg = $conn->receive($sub, array(SAM_WAIT => 10000));

   if (!$msg) {
       echo '<font color="red"><b>FAILED</b></font>  receive failed with errno='.$conn->errno.' '.$conn->error.'<br>';
   } else {
       echo '<font color="green"><b>OK</b></font> msg='.$msg->body.'<br>';
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Attempting unsubscribe -<br>
<?php
if ($conn) {
   if (!$conn->unsubscribe($sub, $queuename)) {
       echo '<font color="red"><b>FAILED</b></font>  unsubscribe failed with errno='.$conn->errno.' '.$conn->error.'<br>';
   } else {
       echo '<font color="green"><b>OK</b></font><br>';
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Check connection still shows as connected -<br>
<?php
if (!$conn->isConnected()) {
    echo '<font color="red"><b>FAILED</b></font>';
} else {
    echo '<font color="green"><b>OK</b></font>   ';
}
?>
<p/>
<hr/>
Attempting disconnect with connected connections -<br>
<?php
if (!$conn->disconnect() || !$conn1->disconnect()) {
    echo '<font color="red"><b>FAILED</b></font>';
} else {
    echo '<font color="green"><b>OK</b></font>   ';
}
?>
<p/>
<hr/>
</body>
</html>