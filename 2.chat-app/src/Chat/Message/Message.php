<?php

namespace App\Chat\Message;

abstract class Message
{
    public function __construct(
        public readonly string $message
    )
    {

    }

    public function __toString()
    {
        return $this->message;
    }
}