<html>
<head>
<title>SAM - WMQ pub/sub Testcase</title>

<?php

// test set up data...
//
$host = 'localhost';
$broker = 'bull';
$port = 1414;
$queuename = 'topic://test/x/y';
$protocol = SAM_WMQ;
//$protocol = SAM_WMQ_CLIENT;
//$protocol = SAM_WMQ_BINDINGS;

?>
</head>

<body>
<h1>SAM - WMQ pub/sub Testcase</h1>
This testcase will exercise the SAM connect, subscribe, receive, unsubscribe and disconnect capabilities
using the WMQ protocol to a WebSphere MQSeries Messaging server.
<p>
The tests will use the following set up:<br>
Protocol:           <b><?php echo $protocol ?></b><br>
Host:               <b><?php echo $host ?></b><br>
Broker:             <b><?php echo $broker ?></b><br>
Port:               <b><?php echo $port ?></b><br>
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
Attempting connection with no option array -<br>
<?php
$conn = new SAMConnection();
if (!$conn->connect(SAM_WMQ)) {
    echo '<font color="green"><b>OK</b></font>   ';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Attempting connection to invalid host -<br>
<?php
if ($protocol == (SAM_WMQ_BINDINGS)) {
    echo '<font color="green"><b>Skipping test as not valid for this protocol and option combination.</b></font>   ';
} else {
    $conn = new SAMConnection();
    if (!$conn->connect($protocol, array(SAM_HOST => 'non-existent_host', SAM_PORT => $port, SAM_BROKER => $broker))) {
        echo '<font color="green"><b>OK</b></font>   ';
    } else {
        echo '<font color="red"><b>FAILED</b></font>';
    }
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

if ($conn1->connect($protocol, array(SAM_HOST => $host, SAM_PORT => $port, SAM_BROKER => $broker))) {
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
if ($conn->connect($protocol, array(SAM_HOST => $host, SAM_PORT => $port, SAM_BROKER => $broker))) {
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