<?php
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;
use Ratchet\Server\IoServer;
use Ratchet\WebSocket\WsServer;
require __DIR__ . '/vendor/autoload.php';

// TODO Justin: rewrite SAM to be PSR-0 compatible
require __DIR__ . ('/vendor/SAM/php_sam.php');

/**
 * control.php
 * 
 * Send any incoming messages to all connected clients and broker.
 * Based on the sample chat application that comes with Rachet
 *
 */
class Control implements MessageComponentInterface {
    protected $clients;
	protected $broker;

    public function __construct() {
        $this->clients = new \SplObjectStorage;
		$this->broker = new SAMConnection();
    }

    public function onOpen(ConnectionInterface $conn) {
        $this->clients->attach($conn);
		
		$this->broker->connect(SAM_MQTT, array(
				SAM_HOST => "ec2-XXX-XXX-XXX-XXX.us-west-1.compute.amazonaws.com", 
				SAM_PORT => 1883
			)
		);
    }

    public function onMessage(ConnectionInterface $from, $msg) {
        foreach ($this->clients as $client) {
		
            if ($from != $client) {
                $client->send($msg);
            }
			
			$msgSAM = new SAMMessage($msg);
 
			$this->broker->send('topic://control', $msgSAM);
        }
    }

    public function onClose(ConnectionInterface $conn) {
        $this->clients->detach($conn);
    }

    public function onError(ConnectionInterface $conn, \Exception $e) {
        $conn->close();
    }
}

// Run the server application through the WebSocket protocol on port 8899
$server = IoServer::factory(new WsServer(new Control), 8899);
$server->run();
