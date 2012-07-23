<html>
<head>
<title>SAM - WPM pub/sub Testcase</title>

<?php

// test set up data...
//

$endpoints = 'localhost:7278:BootstrapBasicMessaging';
$bus = 'Bus1';
$target = 'InboundBasicMessaging';
$queuename = 'topic://test/x/y';
$home = 'RENSHAWDTPNode01.server1-Bus1';

?>
</head>

<body>
<h1>SAM - WPM pub/sub Testcase</h1>
This testcase will exercise the SAM connect, subscribe, receive, unsubscribe and disconnect capabilities
using the WPM protocol to a WebSphere Application Server Messaging system.
<p>
The tests will use the following set up:<br>
Endpoints:          <b><?php echo $endpoints ?></b><br>
Target chain:       <b><?php echo $target ?></b><br>
Bus:                <b><?php echo $bus ?></b><br>
Queue:              <b><?php echo $queuename ?></b><br>
Pub/sub home:       <b><?php echo $home ?></b><br>
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
Attempting connection with no option array -<br>
<?php
$conn = new SAMConnection();
if (!$conn->connect(SAM_WPM)) {
    echo '<font color="green"><b>OK</b></font>   ';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Attempting connection to invalid host -<br>
<?php
$conn = new SAMConnection();
if (!$conn->connect(SAM_WPM, array(SAM_ENDPOINTS => 'non-existant-host:9999:BootstrapBasicMessaging',
                                 SAM_BUS => 'non-existant-bus',
                                 SAM_TARGETCHAIN => 'non-existant-target'))) {
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

if ($conn1->connect(SAM_WPM, array(SAM_ENDPOINTS => $endpoints, SAM_BUS => $bus, SAM_TARGETCHAIN => $target, SAM_WPM_DUR_SUB_HOME => $home))) {
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

if ($conn->connect(SAM_WPM, array(SAM_ENDPOINTS => $endpoints, SAM_BUS => $bus, SAM_TARGETCHAIN => $target, SAM_WPM_DUR_SUB_HOME => $home))) {
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
   $msg = $conn->receive('invalid sub name', array(SAM_WAIT => 5000));

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
   if (!$conn->unsubscribe($sub)) {
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
Attempting disconnect with connected connection -<br>
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