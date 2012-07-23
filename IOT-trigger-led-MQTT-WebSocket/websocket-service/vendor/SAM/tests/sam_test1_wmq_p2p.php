<html>
<head>
<title>SAM - WMQ point to point Testcase</title>

<?php

// test set up data...
//
$host = 'localhost';
$broker = 'bull';
$port = 1414;
$queuename = 'queue://testqueue';
$protocol = SAM_WMQ;
//$protocol = SAM_WMQ_CLIENT;
//$protocol = SAM_WMQ_BINDINGS;

?>
</head>

<body>
<h1>SAM - WMQ Point to Point Testcase</h1>
This testcase will exercise the SAM connect, send, receive, peek, peekAll, remove and disconnect capabilities
using the WMQ protocol to a WebSphere MQSeries Messaging server.
<p>
The tests will use the following set up:<br>
Protocol:           <b><?php echo $protocol ?></b><br>
Host:               <b><?php echo $host ?></b><br>
Broker:             <b><?php echo $broker ?></b><br>
Port:               <b><?php echo $port ?></b><br>
Queue:              <b><?php echo $queuename ?></b><br>
<hr/>
Attempting connection with insufficient parameters -
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
Attempting connection with no option array -
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
Attempting connection to invalid host -
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
Checking unconnected connection shows as unconnected -
<?php
if (!$conn->isConnected()) {
    echo '<font color="green"><b>OK</b></font>   ';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Attempting disconnect with unconnected connection -
<?php
if (!$conn->disconnect()) {
    echo '<font color="green"><b>OK</b></font>   ';
} else {
    echo '<font color="red"><b>FAILED</b></font>';
}
?>
<p/>
<hr/>
Attempting connection to valid host -
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
Attempting a second connection to valid host -
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
Attempting send -
<?php
if ($conn) {
   $msg = new SAMMessage('This is a test message');
   $msg->header->SAM_REPLY_TO = 'queue://test/replyQueue';
   $msg->header->SAM_TYPE = SAM_TEXT;

   $msg->header->myStringProp1 = array('a string property', SAM_STRING);
   $msg->header->myStringProp2 = 'another string property';
   $msg->header->contentType = 'text/xml: charset=utf-8';
   $msg->header->transportVersion = 1;
   $msg->header->myBoolProp = array(FALSE, SAM_BOOL);
   $msg->header->myIntProp = array(32768, SAM_INT);
   $msg->header->myLongProp = array(9876543, SAM_LONG);
   $msg->header->myByteProp1 = array(123, SAM_BYTE);
   $msg->header->myByteProp2 = array('12', SAM_BYTE);
   $msg->header->myFloatProp = array(3.1415926535897932384626433832795028841, SAM_FLOAT);
   $msg->header->myDoubleProp = array(3.1415926535897932384626433832795028841, SAM_DOUBLE);

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
   $correlid2 = $conn->send($queuename, $msg, array(SAM_CORRELID => $correlid));
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
Attempting receive with correlation id -<br/>
<?php
if ($conn) {
   $msg = $conn->receive($queuename, array(SAM_CORRELID => $correlid, SAM_WAIT => 5000));

   if ($msg) {
       foreach ($msg->header as $key => $value) {
           echo 'property <b>'.$key.'</b> set to <b>'.$value.'</b><br/>';
       }
   }

   echo 'Retrieving property by name(JMS_IBM_PutDate) value is -'.$msg->header->JMS_IBM_PutDate.'-<br/>';
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
Attempting receive -<br/>
<?php
if ($conn) {
   $msg = $conn->receive($queuename);

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
Testing peek and remove -<br>
<?php
if ($conn) {
   $m = new SAMMessage('This is a test message to peek');
   $m->header->SAM_TYPE = SAM_TEXT;
   $mid = $conn->send($queuename, $m);
   if (!$mid) {
       echo '<font color="red"><b>FAILED</b></font>'." send failed ($conn->errno) $conn->error";
   } else {
       if (!$conn->peek($queuename, array(SAM_MESSAGEID => $mid, SAM_WAIT => 1000))) {
           echo '<font color="red"><b>FAILED</b></font>'." peek failed ($conn->errno) $conn->error";
       } else {
           if (!$conn->remove($queuename, array(SAM_MESSAGEID => $mid))) {
               echo '<font color="red"><b>FAILED</b></font>'." remove failed ($conn->errno) $conn->error";
           } else {
               echo '<font color="green"><b>OK</b></font> correlID='.$correlid;
           }
       }
   }
} else {
    echo '<font color="red"><b>FAILED</b></font> previous connection create test failed!';
}
?>
<p/>
<hr/>
Testing peekAll -<br>
<?php
if ($conn) {
   $msg = new SAMMessage('This is a test message to peek (1)');
   $msg->header->SAM_TYPE = SAM_TEXT;
   $msgid = $conn->send($queuename, $msg, array(SAM_CORRELID => '1111'));
   if (!$msgid) {
       echo '<font color="red"><b>FAILED</b></font>'." send(1) failed ($conn->errno) $conn->error";
   } else {
       $msg->body = 'This is a test message to peek (2)';
       $msgid = $conn->send($queuename, $msg, array(SAM_CORRELID => '1111'));
       if (!$msgid) {
           echo '<font color="red"><b>FAILED</b></font>'." send(2) failed ($conn->errno) $conn->error";
       } else {
           $msg->body = 'This is a test message to peek (3)';
           $msgid = $conn->send($queuename, $msg, array(SAM_CORRELID => '1111'));
           if (!$msgid) {
               echo '<font color="red"><b>FAILED</b></font>'." send(3) failed ($conn->errno) $conn->error";
           } else {
               $msg->body = 'This is a test message to peek (4)';
               $msgid = $conn->send($queuename, $msg, array(SAM_CORRELID => '1111'));
               if (!$msgid) {
                   echo '<font color="red"><b>FAILED</b></font>'." send(4) failed ($conn->errno) $conn->error";
               } else {
                   $ar = $conn->peekAll($queuename, array(SAM_CORRELID => '1111'));
                   if (!$ar) {
                       echo '<font color="red"><b>FAILED</b></font>'." peekAll failed ($conn->errno) $conn->error";
                   } else {
                       $x = sizeof($ar);
                       if ($x != 4) {
                           echo '<font color="red"><b>FAILED</b></font>'." peekAll failed. Array size=".sizeof($ar)." should be 4!";
                       }
                       foreach($ar as $msg) {
                           if (!$conn->remove($queuename, array(SAM_MESSAGEID => $msg->header->SAM_MESSAGEID))) {
                               echo '<font color="red"><b>FAILED</b></font> remove failed '."($conn->errno) $conn->error";
                           } else {
                               $x -= 1;
                           }
                       }
                       if ($x == 0) {
                           echo '<font color="green"><b>OK</b></font>';
                       }
                   }
               }
           }
       }
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