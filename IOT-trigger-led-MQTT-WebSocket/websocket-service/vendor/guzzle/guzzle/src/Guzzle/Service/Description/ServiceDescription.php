<?php

namespace Guzzle\Service\Description;

use Guzzle\Service\Exception\DescriptionBuilderException;

/**
 * A ServiceDescription stores service information based on a service document
 */
class ServiceDescription implements ServiceDescriptionInterface
{
    /**
     * @var array Array of ApiCommand objects
     */
    protected $commands = array();

    /**
     * @var DescriptionBuilderFactoryInterface
     */
    protected static $defaultFactory;

    /**
     * {@inheritdoc}
     * @param string|array $filename File to build or array of command information
     * @param array $options (optional) Service description factory options
     */
    public static function factory($filename, array $options = null)
    {
        // @codeCoverageIgnoreStart
        if (!self::$defaultFactory) {
            self::$defaultFactory = new ServiceDescriptionFactory();
        }
        // @codeCoverageIgnoreEnd

        return self::$defaultFactory->build($filename);
    }

    /**
     * Create a new ServiceDescription
     *
     * @param array $commands (optional) Array of {@see ApiCommand} objects
     */
    public function __construct(array $commands = array())
    {
        $this->commands = $commands;
    }

    /**
     * Serialize the service description
     *
     * @return string
     */
    public function serialize()
    {
        return json_encode(array_map(function($command) {
            // Convert ApiCommands into arrays
            $data = $command->toArray();
            // Convert ApiParams into arrays
            $data['params'] = array_map(function($param) {
                return $param->toArray();
            }, $data['params']);
            return $data;
        }, $this->commands));
    }

    /**
     * Unserialize the service description
     *
     * @param string|array $json JSON data
     */
    public function unserialize($json)
    {
        $this->commands = array_map(function($data) {
            // Convert params to ApiParam objects
            $data['params'] = array_map(function($param) {
                return new ApiParam($param);
            }, $data['params']);
            // Convert commands into ApiCommands
            return new ApiCommand($data);
        }, json_decode($json, true));
    }

    /**
     * Get the API commands of the service
     *
     * @return array Returns an array of ApiCommand objects
     */
    public function getCommands()
    {
        return $this->commands;
    }

    /**
     * Check if the service has a command by name
     *
     * @param string $name Name of the command to check
     *
     * @return bool
     */
    public function hasCommand($name)
    {
        return array_key_exists($name, $this->commands);
    }

    /**
     * Get an API command by name
     *
     * @param string $name Name of the command
     *
     * @return ApiCommand|null
     */
    public function getCommand($name)
    {
        return $this->hasCommand($name) ? $this->commands[$name] : null;
    }
}
